#include "Deepcache.hpp"

#include "DeepcacheBrowserLogic.hpp"
#include "DeepcacheArchive.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/VisualAssets.hpp"

#include <app/ModuleWidget.hpp>
#include <app/RackWidget.hpp>
#include <app/Scene.hpp>
#include <componentlibrary.hpp>
#include <history.hpp>
#include <plugin.hpp>
#include <settings.hpp>
#include <system.hpp>
#include <tag.hpp>
#include <ui/ChoiceButton.hpp>
#include <ui/Label.hpp>
#include <ui/MenuOverlay.hpp>
#include <ui/OptionButton.hpp>
#include <ui/ScrollWidget.hpp>
#include <ui/SequentialLayout.hpp>
#include <ui/TextField.hpp>
#include <widget/FramebufferWidget.hpp>
#include <widget/TransparentWidget.hpp>
#include <widget/ZoomWidget.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <deque>
#include <exception>
#include <limits>
#include <memory>
#include <map>
#include <mutex>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace rack;

namespace {

class PreviewCacheManager;
struct DeepcacheBrowser;
struct DeepcacheWarmRenderHost;

enum class FramebufferWarmResult {
	READY,
	PENDING_ASSET,
	RETRY,
	FAILED
};

bool fractalOverlaysReadyForCapture(widget::Widget* root) {
	if (!root) return true;
	std::deque<widget::Widget*> queue;
	queue.push_back(root);
	while (!queue.empty()) {
		widget::Widget* current = queue.front();
		queue.pop_front();
		if (auto* overlay =
				dynamic_cast<visual_assets::FractalGlassOverlay*>(current)) {
			if (!overlay->isReadyForCapture()) return false;
		}
		for (widget::Widget* child : current->children) queue.push_back(child);
	}
	return true;
}

void releaseFramebuffers(widget::Widget* root) {
	if (!root)
		return;
	std::deque<widget::Widget*> queue;
	queue.push_back(root);
	while (!queue.empty()) {
		widget::Widget* current = queue.front();
		queue.pop_front();
		if (auto* framebuffer = dynamic_cast<widget::FramebufferWidget*>(current)) {
			framebuffer->setDirty();
			// FramebufferWidget::deleteFramebuffer() requires a live Window. A
			// ContextDestroy event should already have cleared it otherwise.
			if (APP && APP->window)
				framebuffer->deleteFramebuffer();
		}
		for (widget::Widget* child : current->children)
			queue.push_back(child);
	}
}

std::string lowercase(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});
	return value;
}

bool rackModelIsDisplayEligible(const plugin::Model* model) {
	if (!model || !model->plugin || model->hidden)
		return false;
	const settings::ModuleInfo* info = settings::getModuleInfo(model->plugin->slug, model->slug);
	return (!info || info->enabled) &&
	       settings::isModuleWhitelisted(model->plugin->slug, model->slug);
}

int normalizedPreviewCacheResolutionPercent(int percent) {
	return percent >= 150 ? 200 : 100;
}

std::once_flag gDeepcacheSettingsOnce;
std::atomic<int> gDeepcachePreviewCacheResolutionPercent {100};

std::string deepcacheSettingsDirectory() {
	return system::join(asset::user(), "Leviathan/Deepcache");
}

void loadDeepcachePluginSettings() {
	const std::string path = system::join(deepcacheSettingsDirectory(), "settings.json");
	FILE* file = std::fopen(path.c_str(), "r");
	if (!file)
		return;
	json_error_t error = {};
	json_t* root = json_loadf(file, 0, &error);
	std::fclose(file);
	if (!root)
		return;
	if (json_t* value = json_object_get(root, "previewCacheResolutionPercent")) {
		gDeepcachePreviewCacheResolutionPercent.store(
			normalizedPreviewCacheResolutionPercent(static_cast<int>(json_integer_value(value))),
			std::memory_order_relaxed);
	}
	json_decref(root);
}

int deepcachePreviewCacheResolutionPercent() {
	std::call_once(gDeepcacheSettingsOnce, loadDeepcachePluginSettings);
	return normalizedPreviewCacheResolutionPercent(
		gDeepcachePreviewCacheResolutionPercent.load(std::memory_order_relaxed));
}

void saveDeepcachePluginSettings(int previewResolutionPercent) {
	std::call_once(gDeepcacheSettingsOnce, loadDeepcachePluginSettings);
	const int normalized = normalizedPreviewCacheResolutionPercent(previewResolutionPercent);
	gDeepcachePreviewCacheResolutionPercent.store(normalized, std::memory_order_relaxed);
	const std::string directory = deepcacheSettingsDirectory();
	if (!system::createDirectories(directory) && !system::isDirectory(directory)) {
		WARN("Leviathan Deepcache: could not create settings directory: %s", directory.c_str());
		return;
	}
	const std::string path = system::join(directory, "settings.json");
	const std::string temporary = path + ".tmp";
	json_t* root = json_object();
	json_object_set_new(root, "previewCacheResolutionPercent", json_integer(normalized));
	FILE* file = std::fopen(temporary.c_str(), "w");
	if (!file) {
		json_decref(root);
		WARN("Leviathan Deepcache: could not open settings file: %s", temporary.c_str());
		return;
	}
	const int result = json_dumpf(root, file, JSON_INDENT(2));
	const int closeResult = std::fclose(file);
	json_decref(root);
	if (result != 0 || closeResult != 0 || !system::rename(temporary, path)) {
		std::remove(temporary.c_str());
		WARN("Leviathan Deepcache: could not save settings file: %s", path.c_str());
	}
}

std::string pluginArtifactFingerprint(const plugin::Plugin* plugin, int previewResolutionPercent) {
	if (!plugin)
		return "missing-plugin";
	std::uint64_t hash = 1469598103934665603ull;
	auto mix = [&hash](const std::string& value) {
		for (unsigned char c : value) {
			hash ^= c;
			hash *= 1099511628211ull;
		}
	};
	mix("deepcache-raster-schema-3-variable-resolution");
	mix(std::to_string(normalizedPreviewCacheResolutionPercent(previewResolutionPercent)));
	mix(plugin->slug);
	mix(plugin->version);
	mix(plugin->path);
	mix(string::f("%.9f", plugin->modifiedTimestamp));
	mix(settings::preferDarkPanels ? "dark" : "light");
	const char* artifacts[] = {"plugin.dll", "plugin.so", "plugin.dylib", "plugin.json"};
	for (const char* artifact : artifacts) {
		const std::string path = system::join(plugin->path, artifact);
		if (system::isFile(path)) {
			mix(artifact);
			mix(std::to_string(static_cast<unsigned long long>(system::getFileSize(path))));
			struct stat metadata = {};
			if (::stat(path.c_str(), &metadata) == 0) {
				mix(std::to_string(static_cast<long long>(metadata.st_mtime)));
#if defined(__APPLE__)
				mix(std::to_string(static_cast<long long>(metadata.st_mtimespec.tv_nsec)));
#elif !defined(_WIN32)
				mix(std::to_string(static_cast<long long>(metadata.st_mtim.tv_nsec)));
#endif
			}
		}
	}
	return string::f("%016llx", static_cast<unsigned long long>(hash));
}

std::vector<std::string> deepcacheSortNames() {
	return {
		string::translate("Browser.sort.lastUpdated"),
		string::translate("Browser.sort.lastUsed"),
		string::translate("Browser.sort.mostUsed"),
		string::translate("Browser.sort.brand"),
		string::translate("Browser.sort.moduleName"),
		string::translate("Browser.sort.random"),
	};
}

struct ModuleWidgetContainer : widget::Widget {
	void draw(const DrawArgs& args) override {
		Widget::draw(args);
		Widget::drawLayer(args, 1);
	}
};

struct DeepcacheRasterWidget : widget::TransparentWidget {
	std::shared_ptr<const std::vector<std::uint8_t>> rgba;
	int width = 0;
	int height = 0;
	NVGcontext* ownerVg = nullptr;
	int imageHandle = -1;
	int imageWidth = 0;
	int imageHeight = 0;

	~DeepcacheRasterWidget() override {
		NVGcontext* current = APP && APP->window ? APP->window->vg : nullptr;
		nvg_gfx_lifecycle::resetOwnedNvgImage(ownerVg, imageHandle, imageWidth, imageHeight,
		                                         current, ownerVg == current);
	}

	bool ensureImage(NVGcontext* vg) {
		if (!vg || width <= 0 || height <= 0)
			return false;
		if (ownerVg == vg && imageHandle >= 0 &&
		    nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg, imageHandle, width, height))
			return true;
		nvg_gfx_lifecycle::resetOwnedNvgImage(ownerVg, imageHandle, imageWidth, imageHeight,
		                                         vg, ownerVg == vg);
		if (!rgba || rgba->empty())
			return false;
		{
			imageHandle = nvgCreateImageRGBA(vg, width, height, NVG_IMAGE_PREMULTIPLIED, rgba->data());
			if (imageHandle < 0)
				return false;
			ownerVg = vg;
			imageWidth = width;
			imageHeight = height;
		}
		return true;
	}

	bool hasCpuPixels() const {
		return rgba && !rgba->empty();
	}
	std::size_t cpuPixelBytes() const {
		return rgba ? rgba->size() : 0;
	}

	void releaseCpuPixels() {
		rgba.reset();
	}

	void releaseImage() {
		NVGcontext* current = APP && APP->window ? APP->window->vg : nullptr;
		nvg_gfx_lifecycle::resetOwnedNvgImage(ownerVg, imageHandle, imageWidth, imageHeight,
		                                         current, ownerVg == current);
	}

	void draw(const DrawArgs& args) override {
		if (!ensureImage(args.vg))
			return;
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillPaint(args.vg, nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y,
		                                      0.f, imageHandle, 1.f));
		nvgFill(args.vg);
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		nvg_gfx_lifecycle::resetOwnedNvgImage(ownerVg, imageHandle, imageWidth, imageHeight,
		                                         nullptr, false);
		widget::TransparentWidget::onContextDestroy(e);
	}
};

struct DeepcacheModelBox : widget::OpaqueWidget {
	plugin::Model* model = nullptr;
	std::size_t modelIndex = 0;
	int pluginModelOrder = 0;
	PreviewCacheManager* cacheManager = nullptr;
	widget::Widget* previewRoot = nullptr;
	widget::ZoomWidget* zoomWidget = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	app::ModuleWidget* moduleWidget = nullptr;
	ModuleWidgetContainer* moduleContainer = nullptr;
	DeepcacheRasterWidget* rasterWidget = nullptr;
	ui::Tooltip* tooltip = nullptr;
	deepcache::PreviewEntryState state = deepcache::PreviewEntryState::EMPTY;
	std::string failureReason;
	app::ModuleWidget* pendingAddedWidget = nullptr;
	math::Vec pendingDragDelta;

	DeepcacheModelBox(plugin::Model* model, std::size_t modelIndex, int pluginModelOrder,
	                  PreviewCacheManager* manager);
	~DeepcacheModelBox() override;
	bool ensurePreviewConstructed();
	bool installDecodedPreview(deepcache::DecodedPreview preview);
	bool installRasterPreview(std::shared_ptr<const std::vector<std::uint8_t>> rgba, int width, int height);
	bool ensurePersistentImage(NVGcontext* vg);
	bool hasCpuPixels() const;
	std::size_t cpuPixelBytes() const;
	std::uint64_t estimatedGpuBytes() const;
	void releaseCpuPixels();
	void releasePersistentImage();
	bool captureFramebuffer(std::vector<std::uint8_t>& rgba, int& width, int& height) const;
	FramebufferWarmResult warmFramebuffer();
	bool hasValidFramebufferImage() const;
	void invalidateFramebufferReadiness();
	void clearPreview();
	void updateZoom();
	void draw(const DrawArgs& args) override;
	void onButton(const ButtonEvent& e) override;
	void onDragMove(const DragMoveEvent& e) override;
	void onDragEnd(const DragEndEvent& e) override;
	void onEnter(const EnterEvent& e) override;
	void onLeave(const LeaveEvent& e) override;
	void onHide(const HideEvent& e) override;
	void setTooltip(ui::Tooltip* nextTooltip);
	void createContextMenu();
};

struct DeepcacheBrowserSearchField : ui::TextField {
	DeepcacheBrowser* browser = nullptr;
	void step() override;
	void onChange(const ChangeEvent& e) override;
	void onAction(const ActionEvent& e) override;
	void onShow(const ShowEvent& e) override;
	void onHide(const HideEvent& e) override;
};

struct DeepcacheFavoriteQuantity : Quantity {
	DeepcacheBrowser* browser = nullptr;
	void setValue(float value) override;
	float getValue() override;
};

struct DeepcacheZoomButton : ui::ChoiceButton {
	DeepcacheBrowser* browser = nullptr;
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheMulticolumnMenu : ui::Menu {
	math::Vec anchorPos;
	void step() override;
};

struct DeepcacheSingleLineChoiceButton : ui::ChoiceButton {
	void draw(const DrawArgs& args) override;
};

struct DeepcacheBrandItem : ui::MenuItem {
	DeepcacheBrowser* browser = nullptr;
	std::weak_ptr<int> browserLifetime;
	std::string brand;
	int availableModelCount = 0;
	int registeredModelCount = 0;
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheBrandButton : DeepcacheSingleLineChoiceButton {
	DeepcacheBrowser* browser = nullptr;
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheTagItem : ui::MenuItem {
	DeepcacheBrowser* browser = nullptr;
	std::weak_ptr<int> browserLifetime;
	int tagId = -1;
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheTagButton : DeepcacheSingleLineChoiceButton {
	DeepcacheBrowser* browser = nullptr;
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheSlugButton : ui::ChoiceButton {
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheClearButton : ui::Button {
	DeepcacheBrowser* browser = nullptr;
	void onAction(const ActionEvent& e) override;
};

struct DeepcacheSortButton : ui::ChoiceButton {
	DeepcacheBrowser* browser = nullptr;
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheUrlButton : ui::Button {
	std::string url;
	void onAction(const ActionEvent& e) override {
		system::openBrowser(url);
	}
};

struct DeepcacheBrowser : widget::OpaqueWidget {
	PreviewCacheManager* cacheManager = nullptr;
	ui::SequentialLayout* headerLayout = nullptr;
	DeepcacheBrowserSearchField* searchField = nullptr;
	DeepcacheBrandButton* brandButton = nullptr;
	DeepcacheTagButton* tagButton = nullptr;
	DeepcacheFavoriteQuantity* favoriteQuantity = nullptr;
	ui::OptionButton* favoriteButton = nullptr;
	ui::Label* countLabel = nullptr;
	ui::ScrollWidget* modelScroll = nullptr;
	widget::Widget* modelMargin = nullptr;
	ui::SequentialLayout* modelContainer = nullptr;
	std::vector<DeepcacheModelBox*> modelBoxes;
	std::vector<deepcache::ModelDescriptor> modelDescriptors;
	std::vector<deepcache::BrowserModelRecord> browserRecords;
	std::string search;
	std::string brand;
	std::set<int> tagIds;
	bool favoritesOnly = false;
	float lastBrowserZoom = NAN;
	bool lastPreferDarkPanels = false;
	NVGcontext* dragonOwnerVg = nullptr;
	int dragonImageHandle = -1;
	int dragonImageWidth = 0;
	int dragonImageHeight = 0;
	std::shared_ptr<int> lifetimeToken = std::make_shared<int>(0);

	explicit DeepcacheBrowser(PreviewCacheManager* manager);
	~DeepcacheBrowser() override;
	void step() override;
	void draw(const DrawArgs& args) override;
	void onContextDestroy(const ContextDestroyEvent& e) override;
	void onButton(const ButtonEvent& e) override;
	void refresh();
	void clearFilters();
	void sortModels();
	deepcache::BrowserModelRecord makeBrowserRecord(const DeepcacheModelBox* box) const;
	void updateBrowserRecord(const DeepcacheModelBox* box);
	void updateZoom();
	void clearPreviews();
	DeepcacheModelBox* getModelBox(std::size_t index) const;
	std::vector<deepcache::ModelDescriptor> snapshotModelDescriptors() const;
	std::unordered_set<std::size_t> visibleModelIndices() const;
	void writeDisplayEligibility(std::vector<std::uint8_t>* eligibility) const;
	std::size_t residentPreviewCount() const;
	std::size_t framebufferReadyPreviewCount() const;
	void invalidateFramebufferReadiness();
	app::ModuleWidget* chooseModel(plugin::Model* model);
	bool ensureDragonImage(NVGcontext* vg);
	float headerContentWidth() const;
};

class PreviewCacheManager {
public:
	explicit PreviewCacheManager(DeepcacheModule* module)
		: module_(module),
		  lastPreferDarkPanels_(settings::preferDarkPanels),
		  lastPreviewCacheResolutionPercent_(deepcachePreviewCacheResolutionPercent()) {
		publish();
	}

	~PreviewCacheManager() {
		stop();
	}

	void setBrowser(DeepcacheBrowser* browser) {
		browser_ = browser;
		if (browser_)
			browser_->writeDisplayEligibility(&displayEligibility_);
		else
			displayEligibility_.clear();
		displayEligibilityScratch_.resize(displayEligibility_.size());
		lastEligibilityCheckAt_ = system::getTime();
		initializeArchive();
	}

	deepcache::CacheState state() const {
		return state_;
	}

	std::uint64_t generation() const {
		return activeGeneration_;
	}

	int previewCacheResolutionPercent() const {
		return deepcachePreviewCacheResolutionPercent();
	}

	void setPreviewCacheResolutionPercent(int percent) {
		if (!module_ || stopped_)
			return;
		const int normalized = normalizedPreviewCacheResolutionPercent(percent);
		if (previewCacheResolutionPercent() == normalized)
			return;
		saveDeepcachePluginSettings(normalized);
		lastPreviewCacheResolutionPercent_ = normalized;
		onCacheIdentityChanged();
	}

	void start() {
		if (!browser_ || state_ == deepcache::CacheState::STOPPING)
			return;
		if (!archiveReadyForPlanning()) {
			startRequested_ = true;
			setState(deepcache::CacheState::PLANNING);
			return;
		}
		if (activeGeneration_ != 0)
			cancel();
		// clear() rejects late startup decodes. Re-enable archive results only
		// after its handoff queues are drained and a new planner pass truly begins.
		ignoreArchiveResults_ = false;

		activeGeneration_ = ++nextGeneration_;
		completed_ = 0;
		total_ = 0;
		failed_ = 0;
		constructionTarget_ = 0;
		constructionCompleted_ = 0;
		constructionFailed_ = 0;
		constructionPluginRemaining_.clear();
		constructionCountedIndices_.clear();
		constructionPluginTarget_ = 0;
		constructionPluginCompleted_ = 0;
		generationResidentIndices_.clear();
		constructionTotalMs_ = 0.0;
		constructionMaxMs_ = 0.0;
		constructedCount_ = 0;
		warmingStartedAt_ = system::getTime();
		deepcache::PreviewPlanInput input;
		input.generation = activeGeneration_;
		input.visibleModelIndices = browser_->visibleModelIndices();
		input.indexedModelIndices = compressedModelIndices_;
		std::vector<deepcache::ModelDescriptor> descriptors = browser_->snapshotModelDescriptors();
		descriptors.erase(std::remove_if(descriptors.begin(), descriptors.end(), [this](const deepcache::ModelDescriptor& descriptor) {
			DeepcacheModelBox* box = browser_->getModelBox(descriptor.modelIndex);
			return compressedModelIndices_.count(descriptor.modelIndex) != 0 ||
			       (box && box->state == deepcache::PreviewEntryState::FRAMEBUFFER_READY);
		}), descriptors.end());
		for (const deepcache::ModelDescriptor& descriptor : descriptors)
			constructionPluginRemaining_[descriptor.pluginSlug]++;
		constructionPluginTarget_ = static_cast<int>(constructionPluginRemaining_.size());
		worker_.resume();
		worker_.submit(std::move(descriptors), std::move(input));
		setState(deepcache::CacheState::PLANNING);
	}

	void rebuild() {
		clear();
		archive_.requestReset();
		start();
	}

	void cancel() {
		startRequested_ = false;
		if (activeGeneration_ != 0)
			worker_.cancel(activeGeneration_);
		activeGeneration_ = 0;
		completed_ = 0;
		total_ = 0;
		failed_ = 0;
		constructionTarget_ = 0;
		constructionCompleted_ = 0;
		constructionFailed_ = 0;
		framebufferTarget_ = 0;
		framebufferCompleted_ = 0;
		warmTrackedGeneration_.clear();
		generationResidentIndices_.clear();
		constructionPluginRemaining_.clear();
		constructionCountedIndices_.clear();
		constructionPluginTarget_ = 0;
		constructionPluginCompleted_ = 0;
		setState(deepcache::CacheState::IDLE);
	}

	void clear() {
		if (!browser_)
			return;
		startRequested_ = false;
		if (activeGeneration_ != 0)
			worker_.cancel(activeGeneration_);
		activeGeneration_ = 0;
		setState(deepcache::CacheState::CLEARING);
		browser_->clearPreviews();
		backend_.clear();
		archive_.discardPendingDecodes();
		persistentModelIndices_.clear();
		compressedModelIndices_.clear();
		rehydrationPendingIndices_.clear();
		restoreDecodeRequestedIndices_.clear();
		restoreUploadQueuedIndices_.clear();
		persistentUploadQueue_.clear();
		archiveWriteRetryQueue_.clear();
		pendingUploadBytes_ = 0;
		graphicsContextLost_ = false;
		graphicsRestoreScheduled_ = false;
		ignoreArchiveResults_ = true;
		completed_ = 0;
		total_ = 0;
		failed_ = 0;
		constructionTarget_ = 0;
		constructionCompleted_ = 0;
		constructionFailed_ = 0;
		framebufferTarget_ = 0;
		framebufferCompleted_ = 0;
		framebufferWarmQueue_.clear();
		warmQueuedIndices_.clear();
		warmTrackedGeneration_.clear();
		onDemandBuildQueue_.clear();
		onDemandQueuedIndices_.clear();
		generationResidentIndices_.clear();
		constructionPluginRemaining_.clear();
		constructionCountedIndices_.clear();
		constructionPluginTarget_ = 0;
		constructionPluginCompleted_ = 0;
		resetFramebufferPluginProgress();
		setState(deepcache::CacheState::IDLE);
	}

	void stop() {
		if (stopped_)
			return;
		stopped_ = true;
		state_ = deepcache::CacheState::STOPPING;
		publish();
		worker_.shutdown();
		archiveWriteRetryQueue_.clear();
		archive_.shutdown();
		activeGeneration_ = 0;
		browser_ = nullptr;
		lifetimeToken_.reset();
	}

	std::weak_ptr<int> lifetimeToken() const {
		return lifetimeToken_;
	}

	void promote(std::size_t modelIndex) {
		if (activeGeneration_ != 0)
			worker_.promote(modelIndex, activeGeneration_);
	}

	void promote(const std::unordered_set<std::size_t>& modelIndices) {
		if (activeGeneration_ != 0)
			worker_.promote(modelIndices, activeGeneration_);
	}

	void requestOnDemand(std::size_t modelIndex) {
		if (stopped_ || !browser_)
			return;
		DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
		if (!box || (box->state != deepcache::PreviewEntryState::EMPTY &&
		             box->state != deepcache::PreviewEntryState::QUEUED))
			return;
		if (activeGeneration_ != 0)
			worker_.promote(modelIndex, activeGeneration_);
		box->state = deepcache::PreviewEntryState::QUEUED;
		if (onDemandQueuedIndices_.insert(modelIndex).second)
			onDemandBuildQueue_.push_back(modelIndex);
	}

	void retryPreview(std::size_t modelIndex) {
		if (stopped_ || !browser_)
			return;
		DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
		if (!box || box->state != deepcache::PreviewEntryState::FAILED)
			return;
		box->state = deepcache::PreviewEntryState::EMPTY;
		box->failureReason.clear();
		requestOnDemand(modelIndex);
	}

	void onCacheIdentityChanged() {
		const bool restart = startRequested_ || activeGeneration_ != 0 || state_ == deepcache::CacheState::PLANNING ||
		                     state_ == deepcache::CacheState::WARMING || state_ == deepcache::CacheState::PAUSED ||
		                     state_ == deepcache::CacheState::READY;
		archive_.discardPendingWrites();
		clear();
		std::unordered_map<const plugin::Plugin*, std::string> pluginFingerprints;
		for (std::size_t modelIndex = 0; modelIndex < browser_->modelBoxes.size(); ++modelIndex) {
			DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
			if (!box || !box->model || !box->model->plugin)
				continue;
			const plugin::Plugin* plugin = box->model->plugin;
			auto fingerprint = pluginFingerprints.find(plugin);
			if (fingerprint == pluginFingerprints.end())
				fingerprint = pluginFingerprints.emplace(
					plugin, pluginArtifactFingerprint(plugin, previewCacheResolutionPercent())).first;
			fingerprintByModelIndex_[modelIndex] = fingerprint->second;
			if (modelIndex < browser_->modelDescriptors.size())
				browser_->modelDescriptors[modelIndex].artifactFingerprint = fingerprint->second;
		}
		if (restart)
			start();
	}

	bool isDisplayEligible(std::size_t modelIndex) const {
		return modelIndex < displayEligibility_.size() && displayEligibility_[modelIndex] != 0u;
	}

	void reconcileDisplayEligibility(bool force = false) {
		if (!browser_)
			return;
		const double now = system::getTime();
		if (!force && std::isfinite(lastEligibilityCheckAt_) && now - lastEligibilityCheckAt_ < 0.75)
			return;
		lastEligibilityCheckAt_ = now;
		browser_->writeDisplayEligibility(&displayEligibilityScratch_);
		if (displayEligibilityScratch_ == displayEligibility_)
			return;

		const std::size_t modelCount = std::min(displayEligibility_.size(), displayEligibilityScratch_.size());
		for (std::size_t modelIndex = 0; modelIndex < modelCount; ++modelIndex) {
			if (displayEligibility_[modelIndex] == 0u || displayEligibilityScratch_[modelIndex] != 0u)
				continue;
			rehydrationPendingIndices_.erase(modelIndex);
			DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
			if (box) {
				box->releasePersistentImage();
				if (compressedModelIndices_.count(modelIndex) != 0 &&
				    restoreUploadQueuedIndices_.count(modelIndex) == 0)
					box->releaseCpuPixels();
			}
		}

		displayEligibility_.swap(displayEligibilityScratch_);
		// Refresh the browser's fundamental visibility flags too. Search, brand,
		// tag, and favorite filters remain unchanged.
		browser_->refresh();
		graphicsRestoreScheduled_ = false;
		for (std::size_t modelIndex = 0; modelIndex < modelCount; ++modelIndex) {
			if (displayEligibility_[modelIndex] == 0u || displayEligibilityScratch_[modelIndex] != 0u)
				continue;
			DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
			if (!box || box->hasValidFramebufferImage())
				continue;
			box->invalidateFramebufferReadiness();
			if (box->hasCpuPixels()) {
				queuePersistentUpload(modelIndex);
			}
			else if (compressedModelIndices_.count(modelIndex) != 0) {
				persistentModelIndices_.insert(modelIndex);
				rehydrationPendingIndices_.insert(modelIndex);
			}
			else if (box->state == deepcache::PreviewEntryState::RESIDENT && box->framebuffer) {
				queueFramebufferWarm(modelIndex, false, 0);
			}
			else if (archive_.state() != deepcache::DatabaseState::LOADING) {
				requestOnDemand(modelIndex);
			}
		}
		resetFramebufferPluginProgress();
		if ((!rehydrationPendingIndices_.empty() || !persistentUploadQueue_.empty()) &&
		    state_ == deepcache::CacheState::READY)
			setState(deepcache::CacheState::WARMING);
		else
			publish();
	}

	void onGraphicsContextDestroy() {
		if (!browser_)
			return;
		graphicsContextLost_ = true;
		++graphicsGeneration_;
		archive_.discardPendingDecodes();
		browser_->invalidateFramebufferReadiness();
		persistentUploadQueue_.clear();
		pendingUploadBytes_ = 0;
		restoreDecodeRequestedIndices_.clear();
		restoreUploadQueuedIndices_.clear();
		graphicsRestoreScheduled_ = false;
		rehydrationPendingIndices_.clear();
		for (std::size_t modelIndex : persistentModelIndices_) {
			if (isDisplayEligible(modelIndex))
				rehydrationPendingIndices_.insert(modelIndex);
		}
		resetFramebufferPluginProgress();
		if (!rehydrationPendingIndices_.empty() && state_ == deepcache::CacheState::READY)
			setState(deepcache::CacheState::WARMING);
		else
			publish();
	}

	void step() {
		if (stopped_ || !browser_)
			return;
		reconcileDisplayEligibility();
		bool cacheIdentityChanged = false;
		if (settings::preferDarkPanels != lastPreferDarkPanels_) {
			lastPreferDarkPanels_ = settings::preferDarkPanels;
			cacheIdentityChanged = true;
		}
		const int requestedResolution = previewCacheResolutionPercent();
		if (requestedResolution != lastPreviewCacheResolutionPercent_) {
			lastPreviewCacheResolutionPercent_ = requestedResolution;
			cacheIdentityChanged = true;
		}
		if (cacheIdentityChanged)
			onCacheIdentityChanged();
		drainArchiveCommits();
		drainArchiveIndexedCandidates();
		drainArchiveDecoded();
		publishDatabase();
		if (archive_.state() == deepcache::DatabaseState::ERROR) {
			startRequested_ = false;
			if (activeGeneration_ != 0)
				worker_.cancel(activeGeneration_);
			activeGeneration_ = 0;
			if (state_ != deepcache::CacheState::ERROR)
				setState(deepcache::CacheState::ERROR);
			return;
		}
		if (startRequested_ && archiveReadyForPlanning()) {
			startRequested_ = false;
			start();
			return;
		}

		if (state_ == deepcache::CacheState::PLANNING && activeGeneration_ != 0 &&
		    worker_.hasPlanFailed(activeGeneration_)) {
			failed_++;
			setState(deepcache::CacheState::ERROR);
			return;
		}

		if (state_ == deepcache::CacheState::PLANNING && activeGeneration_ != 0 &&
		    worker_.isPlanReady(activeGeneration_)) {
			total_ = static_cast<int>(worker_.plannedRequestCount(activeGeneration_));
			constructionTarget_ = total_;
			if (total_ == 0)
				setState(deepcache::CacheState::READY);
			else
				setState(deepcache::CacheState::WARMING);
		}

		const double frameStart = system::getTime();
		const double budgetMs = std::max(0.5, module_->uiBudgetMicros.load(std::memory_order_relaxed) / 1000.0);
		int processedThisFrame = 0;
		while (processedThisFrame < 4) {
			if (processedThisFrame > 0 && (system::getTime() - frameStart) * 1000.0 >= budgetMs)
				break;
			deepcache::PreviewBuildRequest request;
			bool tracked = false;
			if (!onDemandBuildQueue_.empty()) {
				request.modelIndex = onDemandBuildQueue_.front();
				onDemandBuildQueue_.pop_front();
				onDemandQueuedIndices_.erase(request.modelIndex);
				const auto key = cacheKeyByModelIndex_.find(request.modelIndex);
				if (key != cacheKeyByModelIndex_.end())
					request.cacheKey = key->second;
			}
			else {
				if (state_ != deepcache::CacheState::WARMING || !worker_.tryPop(request))
					break;
				if (request.generation != activeGeneration_)
					continue;
				tracked = true;
			}
			DeepcacheModelBox* box = browser_->getModelBox(request.modelIndex);
			if (!box) {
				if (tracked) {
					constructionCompleted_++;
					constructionFailed_++;
					completed_++;
					failed_++;
					markConstructionModelComplete(request.modelIndex);
				}
				processedThisFrame++;
				publish();
				continue;
			}

			const bool wasComplete = box->state == deepcache::PreviewEntryState::RESIDENT ||
			                         box->state == deepcache::PreviewEntryState::FRAMEBUFFER_READY ||
			                         box->state == deepcache::PreviewEntryState::FAILED;
			const double startedAt = system::getTime();
			const bool resident = wasComplete ? box->state != deepcache::PreviewEntryState::FAILED
			                                  : box->ensurePreviewConstructed();
			const double durationMs = (system::getTime() - startedAt) * 1000.0;
			if (!wasComplete) {
				constructedCount_++;
				constructionTotalMs_ += durationMs;
				constructionMaxMs_ = std::max(constructionMaxMs_, durationMs);
				if (isDragonKingDebugEnabled() && durationMs >= 16.0) {
					WARN("Leviathan Deepcache: slow preview %.2f ms: %s", durationMs,
					     box->model ? box->model->getFullName().c_str() : "unknown");
				}
			}
			if (!resident) {
				if (tracked) {
					failed_++;
					constructionFailed_++;
				}
				backend_.invalidate(request.cacheKey);
			}
			else {
				if (tracked)
					generationResidentIndices_.insert(request.modelIndex);
				backend_.store(request.cacheKey);
				if (box->state == deepcache::PreviewEntryState::FRAMEBUFFER_READY) {
					if (tracked)
						completed_++;
					markFramebufferModelComplete(request.modelIndex);
				}
				else
					queueFramebufferWarm(request.modelIndex, tracked, request.generation);
			}
			if (tracked) {
				constructionCompleted_++;
				if (!resident)
					completed_++;
				markConstructionModelComplete(request.modelIndex);
			}
			processedThisFrame++;
			publish();
		}
		if (constructionCompleted_ >= constructionTarget_) {
			publish();
		}
		finishIfComplete();
	}

	void warmFramebuffers() {
		drainArchiveWriteRetries();
		scheduleGraphicsRestore();
		uploadPersistentImages();
		if (stopped_ || !browser_ || !archiveWriteRetryQueue_.empty() ||
		    framebufferWarmQueue_.empty()) {
			finishIfComplete();
			return;
		}
		const double frameStart = system::getTime();
		const double budgetMs = std::max(0.5, module_->uiBudgetMicros.load(std::memory_order_relaxed) / 1000.0);
		int processedThisFrame = 0;
		while (processedThisFrame < 4 && !framebufferWarmQueue_.empty()) {
			if (!archiveWriteRetryQueue_.empty() || !archive_.canAcceptWrite())
				break;
			if (processedThisFrame > 0 && (system::getTime() - frameStart) * 1000.0 >= budgetMs)
				break;
			auto request = framebufferWarmQueue_.front();
			framebufferWarmQueue_.pop_front();
			DeepcacheModelBox* box = browser_->getModelBox(request.first);
			const double startedAt = system::getTime();
			FramebufferWarmResult result = box ? box->warmFramebuffer() : FramebufferWarmResult::FAILED;
			if (result == FramebufferWarmResult::READY && box && box->framebuffer) {
				std::vector<std::uint8_t> captured;
				int width = 0;
				int height = 0;
				if (!box->captureFramebuffer(captured, width, height)) {
					result = FramebufferWarmResult::RETRY;
				}
				else {
					auto pixels = std::make_shared<const std::vector<std::uint8_t>>(std::move(captured));
					if (!box->installRasterPreview(pixels, width, height)) {
						result = FramebufferWarmResult::FAILED;
					}
					else {
						persistentModelIndices_.insert(request.first);
						const auto key = cacheKeyByModelIndex_.find(request.first);
						if (key != cacheKeyByModelIndex_.end()) {
							deepcache::PreviewWrite write;
							write.cacheKey = key->second;
							write.fingerprint = fingerprintByModelIndex_[request.first];
							write.width = width;
							write.height = height;
							write.rgba = pixels;
							submitArchiveWrite(std::move(write));
						}
						if (isDisplayEligible(request.first)) {
							result = box->ensurePersistentImage(APP && APP->window ? APP->window->vg : nullptr)
							       ? FramebufferWarmResult::READY : FramebufferWarmResult::RETRY;
						}
						else {
							// The temporary framebuffer was needed to create the QOI, but an
							// unavailable model must not retain a persistent GPU image.
							box->releasePersistentImage();
							result = FramebufferWarmResult::READY;
						}
					}
				}
			}
			const double durationMs = (system::getTime() - startedAt) * 1000.0;
			processedThisFrame++;
			const int retryLimit =
				result == FramebufferWarmResult::PENDING_ASSET ? 120 : 3;
			if ((result == FramebufferWarmResult::RETRY ||
					result == FramebufferWarmResult::PENDING_ASSET) &&
				request.second < retryLimit) {
				request.second++;
				framebufferWarmQueue_.push_back(request);
			}
			else {
				warmQueuedIndices_.erase(request.first);
				const auto tracked = warmTrackedGeneration_.find(request.first);
				const bool countsForActiveGeneration = tracked != warmTrackedGeneration_.end() &&
				                                      tracked->second == activeGeneration_ && activeGeneration_ != 0;
				warmTrackedGeneration_.erase(request.first);
				if (countsForActiveGeneration) {
					completed_++;
					if (result != FramebufferWarmResult::READY)
						failed_++;
				}
				if (result == FramebufferWarmResult::READY)
					markFramebufferModelComplete(request.first);
				else if (box && box->framebuffer)
					box->clearPreview();
				if (isDragonKingDebugEnabled() && durationMs >= 16.0) {
					WARN("Leviathan Deepcache: slow framebuffer warm %.2f ms: %s", durationMs,
					     box && box->model ? box->model->getFullName().c_str() : "unknown");
				}
			}
			publish();
		}
		finishIfComplete();
	}

	int completedCount() const { return completed_; }
	int totalCount() const { return total_; }
	int failedCount() const { return failed_; }
	std::size_t residentRecordCount() const { return backend_.size(); }
	std::size_t residentPreviewCount() const { return browser_ ? browser_->residentPreviewCount() : 0; }
	std::size_t framebufferReadyPreviewCount() const { return browser_ ? browser_->framebufferReadyPreviewCount() : 0; }
	std::uint64_t retainedRgbaBytes() const {
		std::uint64_t bytes = 0;
		if (browser_) {
			for (const DeepcacheModelBox* box : browser_->modelBoxes)
				bytes += box ? static_cast<std::uint64_t>(box->cpuPixelBytes()) : 0;
		}
		return bytes;
	}
	std::uint64_t estimatedGpuBytes() const {
		std::uint64_t bytes = 0;
		if (browser_) {
			for (const DeepcacheModelBox* box : browser_->modelBoxes)
				bytes += box ? box->estimatedGpuBytes() : 0;
		}
		return bytes;
	}
	std::uint64_t hotQoiBytes() const { return archive_.hotCompressedBytes(); }
	std::size_t pendingUploadBytes() const { return pendingUploadBytes_; }
	double averageConstructionMs() const { return constructedCount_ > 0 ? constructionTotalMs_ / constructedCount_ : 0.0; }
	double maximumConstructionMs() const { return constructionMaxMs_; }

private:
	void finishIfComplete() {
		if (state_ != deepcache::CacheState::WARMING || constructionCompleted_ < constructionTarget_ ||
		    worker_.pendingRequestCount(activeGeneration_) != 0 || completed_ < total_ ||
		    !warmTrackedGeneration_.empty() || !rehydrationPendingIndices_.empty() ||
		    !archiveWriteRetryQueue_.empty())
			return;
		if (isDragonKingDebugEnabled()) {
			const double elapsedMs = (system::getTime() - warmingStartedAt_) * 1000.0;
			const double averageMs = constructedCount_ > 0 ? constructionTotalMs_ / constructedCount_ : 0.0;
			INFO("Leviathan Deepcache: ready %d/%d, failed=%d, framebufferReady=%llu, elapsed=%.2f ms, average=%.2f ms, max=%.2f ms",
			     completed_, total_, failed_,
			     static_cast<unsigned long long>(framebufferReadyPreviewCount()),
			     elapsedMs, averageMs, constructionMaxMs_);
		}
		setState(deepcache::CacheState::READY);
	}

	void setState(deepcache::CacheState next) {
		if (!deepcache::isValidTransition(state_, next) && state_ != deepcache::CacheState::DISABLED) {
			WARN("Leviathan Deepcache: invalid cache state transition %d -> %d",
			     static_cast<int>(state_), static_cast<int>(next));
		}
		state_ = next;
		publish();
	}

	void publish() {
		if (!module_)
			return;
		module_->cacheState.store(static_cast<int>(state_), std::memory_order_relaxed);
		module_->completedCount.store(completed_, std::memory_order_relaxed);
		module_->totalCount.store(total_, std::memory_order_relaxed);
		module_->failedCount.store(failed_, std::memory_order_relaxed);
		module_->constructionPluginCompletedCount.store(constructionPluginCompleted_, std::memory_order_relaxed);
		module_->constructionPluginTotalCount.store(constructionPluginTarget_, std::memory_order_relaxed);
		module_->framebufferPluginCompletedCount.store(framebufferCompleted_, std::memory_order_relaxed);
		module_->framebufferPluginTotalCount.store(framebufferTarget_, std::memory_order_relaxed);
		publishDatabase();
	}

	void publishDatabase() {
		if (!module_)
			return;
		module_->databaseState.store(static_cast<int>(archive_.state()), std::memory_order_relaxed);
		module_->databaseReadyPluginCount.store(archive_.readyPluginCount(), std::memory_order_relaxed);
		module_->databaseTargetPluginCount.store(archive_.targetPluginCount(), std::memory_order_relaxed);
		module_->databaseBytes.store(archive_.packBytes(), std::memory_order_relaxed);
		module_->databaseErrorCode.store(archive_.errorCode(), std::memory_order_relaxed);
	}

	bool archiveReadyForPlanning() const {
		const deepcache::DatabaseState databaseState = archive_.state();
		return databaseState != deepcache::DatabaseState::ERROR &&
		       archive_.indexDiscoveryComplete() &&
		       !archive_.hasPendingIndexedCandidates();
	}

	bool submitArchiveWrite(deepcache::PreviewWrite write) {
		const std::size_t byteCount = write.rgba ? write.rgba->size() : 0;
		if (archive_.canAcceptWrite(byteCount) && archive_.enqueue(write))
			return true;
		// Warming stops while this queue is nonempty, so it remains bounded to
		// the one captured raster that lost admission to a concurrently changing
		// archive queue or worker state.
		archiveWriteRetryQueue_.push_back(std::move(write));
		return false;
	}

	void drainArchiveWriteRetries() {
		int submitted = 0;
		while (!archiveWriteRetryQueue_.empty() && submitted < 16) {
			const deepcache::PreviewWrite& write = archiveWriteRetryQueue_.front();
			const std::size_t byteCount = write.rgba ? write.rgba->size() : 0;
			if (!archive_.canAcceptWrite(byteCount) || !archive_.enqueue(write))
				break;
			archiveWriteRetryQueue_.pop_front();
			submitted++;
		}
	}

	void initializeArchive() {
		if (!browser_ || archiveStarted_)
			return;
		archiveStarted_ = true;
		const std::string directory = system::join(asset::user(), "Leviathan/Deepcache");
		if (!system::createDirectories(directory) && !system::isDirectory(directory)) {
			WARN("Leviathan Deepcache: could not create database directory: %s", directory.c_str());
			archive_.markUnavailable(7);
			publishDatabase();
			return;
		}
		std::vector<deepcache::ArchiveWantedEntry> wanted;
		for (const deepcache::ModelDescriptor& descriptor : browser_->snapshotModelDescriptors()) {
			const std::string key = deepcache::makePreviewCacheKey(descriptor);
			cacheKeyByModelIndex_[descriptor.modelIndex] = key;
			fingerprintByModelIndex_[descriptor.modelIndex] = descriptor.artifactFingerprint;
			modelIndexByCacheKey_[key] = descriptor.modelIndex;
			modelPluginKeyByIndex_[descriptor.modelIndex] = descriptor.pluginSlug;
			wanted.push_back({key, descriptor.artifactFingerprint, descriptor.pluginSlug});
		}
		resetFramebufferPluginProgress();
		archive_.start(directory, std::move(wanted));
		publishDatabase();
	}

	void drainArchiveCommits() {
		std::string cacheKey;
		int drained = 0;
		while (drained < 64 && archive_.tryPopCommitted(cacheKey)) {
			const auto found = modelIndexByCacheKey_.find(cacheKey);
			if (found != modelIndexByCacheKey_.end()) {
				const std::size_t modelIndex = found->second;
				compressedModelIndices_.insert(modelIndex);
				persistentModelIndices_.insert(modelIndex);
				DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
				// The archive commit is now the recovery source. Once the current
				// context owns a valid image, the full RGBA allocation is redundant.
				if (box && !isDisplayEligible(modelIndex)) {
					box->releasePersistentImage();
					if (restoreUploadQueuedIndices_.count(modelIndex) == 0)
						box->releaseCpuPixels();
				}
				else if (!graphicsContextLost_ && box && box->hasValidFramebufferImage()) {
					box->releaseCpuPixels();
				}
				else if (box && box->hasCpuPixels()) {
					queuePersistentUpload(modelIndex);
				}
			}
			cacheKey.clear();
			drained++;
		}
	}

	void drainArchiveIndexedCandidates() {
		deepcache::IndexedCandidate candidate;
		int drained = 0;
		while (drained < 256 && archive_.tryPopIndexedCandidate(candidate)) {
			const auto found = modelIndexByCacheKey_.find(candidate.cacheKey);
			if (found != modelIndexByCacheKey_.end()) {
				compressedModelIndices_.insert(found->second);
				persistentModelIndices_.insert(found->second);
			}
			candidate = deepcache::IndexedCandidate();
			drained++;
		}
	}

	bool queuePersistentUpload(std::size_t modelIndex) {
		if (!isDisplayEligible(modelIndex))
			return false;
		DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
		if (!box || !box->hasCpuPixels())
			return false;
		if (!restoreUploadQueuedIndices_.insert(modelIndex).second)
			return true;
		const std::size_t bytes = box->cpuPixelBytes();
		pendingUploadBytes_ = bytes <= std::numeric_limits<std::size_t>::max() - pendingUploadBytes_
		                    ? pendingUploadBytes_ + bytes : std::numeric_limits<std::size_t>::max();
		persistentUploadQueue_.push_back({modelIndex, 0});
		return true;
	}

	void finishPersistentUpload(std::size_t modelIndex, DeepcacheModelBox* box) {
		if (restoreUploadQueuedIndices_.erase(modelIndex) == 0)
			return;
		const std::size_t bytes = box ? box->cpuPixelBytes() : 0;
		pendingUploadBytes_ = bytes <= pendingUploadBytes_ ? pendingUploadBytes_ - bytes : 0;
	}

	void scheduleGraphicsRestore() {
		if (graphicsRestoreScheduled_ || !APP || !APP->window || !APP->window->vg ||
		    rehydrationPendingIndices_.empty())
			return;
		graphicsRestoreScheduled_ = true;
		graphicsContextLost_ = false;
		const std::unordered_set<std::size_t> visible = browser_->visibleModelIndices();
		std::vector<std::size_t> ordered;
		ordered.reserve(rehydrationPendingIndices_.size());
		for (std::size_t modelIndex : rehydrationPendingIndices_) {
			if (visible.count(modelIndex) != 0)
				ordered.push_back(modelIndex);
		}
		for (std::size_t modelIndex : rehydrationPendingIndices_) {
			if (visible.count(modelIndex) == 0)
				ordered.push_back(modelIndex);
		}
		std::vector<std::size_t> rebuild;
		for (std::size_t modelIndex : ordered) {
			if (!isDisplayEligible(modelIndex)) {
				rehydrationPendingIndices_.erase(modelIndex);
				continue;
			}
			DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
			if (!box) {
				rebuild.push_back(modelIndex);
				continue;
			}
			if (box->hasCpuPixels()) {
				queuePersistentUpload(modelIndex);
				continue;
			}
			if (compressedModelIndices_.count(modelIndex) != 0) {
				const auto key = cacheKeyByModelIndex_.find(modelIndex);
				if (key != cacheKeyByModelIndex_.end() &&
				    restoreDecodeRequestedIndices_.insert(modelIndex).second) {
					if (!archive_.requestDecode(key->second, graphicsGeneration_)) {
						restoreDecodeRequestedIndices_.erase(modelIndex);
						rebuild.push_back(modelIndex);
					}
				}
				else if (key == cacheKeyByModelIndex_.end()) {
					rebuild.push_back(modelIndex);
				}
				continue;
			}
			rebuild.push_back(modelIndex);
		}
		for (std::size_t modelIndex : rebuild) {
			rehydrationPendingIndices_.erase(modelIndex);
			persistentModelIndices_.erase(modelIndex);
			DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
			if (box)
				box->clearPreview();
			requestOnDemand(modelIndex);
		}
	}

	void drainArchiveDecoded() {
		const double startedAt = system::getTime();
		const double budgetMs = std::max(0.5, module_->uiBudgetMicros.load(std::memory_order_relaxed) / 1000.0);
		int drained = 0;
		deepcache::DecodedPreview preview;
		while (drained < 16 && restoreUploadQueuedIndices_.size() < 16 &&
		       pendingUploadBytes_ < 64u * 1024u * 1024u && archive_.tryPopDecoded(preview)) {
			if (ignoreArchiveResults_ || preview.decodeGeneration != graphicsGeneration_) {
				preview = deepcache::DecodedPreview();
			}
			else {
				const auto found = modelIndexByCacheKey_.find(preview.cacheKey);
				if (found != modelIndexByCacheKey_.end()) {
					const std::size_t modelIndex = found->second;
					restoreDecodeRequestedIndices_.erase(modelIndex);
					DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
					if (preview.rgba.empty()) {
						compressedModelIndices_.erase(modelIndex);
						persistentModelIndices_.erase(modelIndex);
						rehydrationPendingIndices_.erase(modelIndex);
						if (box)
							box->clearPreview();
						requestOnDemand(modelIndex);
					}
					else {
						compressedModelIndices_.insert(modelIndex);
						persistentModelIndices_.insert(modelIndex);
						if (!isDisplayEligible(modelIndex)) {
							rehydrationPendingIndices_.erase(modelIndex);
						}
						else if (box && box->installDecodedPreview(std::move(preview))) {
							queuePersistentUpload(modelIndex);
						}
					}
				}
			}
			preview = deepcache::DecodedPreview();
			drained++;
			if ((system::getTime() - startedAt) * 1000.0 >= budgetMs)
				break;
		}
	}

	void uploadPersistentImages() {
		if (!APP || !APP->window || !APP->window->vg || persistentUploadQueue_.empty())
			return;
		const double startedAt = system::getTime();
		const double budgetMs = std::max(0.5, module_->uiBudgetMicros.load(std::memory_order_relaxed) / 1000.0);
		int uploaded = 0;
		const std::size_t queuedAtStart = persistentUploadQueue_.size();
		while (!persistentUploadQueue_.empty() && uploaded < 16 &&
		       static_cast<std::size_t>(uploaded) < queuedAtStart) {
			if (uploaded > 0 && (system::getTime() - startedAt) * 1000.0 >= budgetMs)
				break;
			const auto request = persistentUploadQueue_.front();
			persistentUploadQueue_.pop_front();
			const std::size_t modelIndex = request.first;
			DeepcacheModelBox* box = browser_->getModelBox(modelIndex);
			if (!isDisplayEligible(modelIndex)) {
				finishPersistentUpload(modelIndex, box);
				rehydrationPendingIndices_.erase(modelIndex);
				if (box) {
					box->releasePersistentImage();
					if (compressedModelIndices_.count(modelIndex) != 0)
						box->releaseCpuPixels();
				}
			}
			else if (box && box->ensurePersistentImage(APP->window->vg)) {
				const auto key = cacheKeyByModelIndex_.find(modelIndex);
				if (key != cacheKeyByModelIndex_.end())
					backend_.store(key->second);
				markFramebufferModelComplete(modelIndex);
				finishPersistentUpload(modelIndex, box);
				rehydrationPendingIndices_.erase(modelIndex);
				if (compressedModelIndices_.count(modelIndex) != 0)
					box->releaseCpuPixels();
			}
			else if (box && request.second < 3) {
				persistentUploadQueue_.push_back({modelIndex, request.second + 1});
			}
			else {
				finishPersistentUpload(modelIndex, box);
				rehydrationPendingIndices_.erase(modelIndex);
			}
			uploaded++;
		}
		publish();
		finishIfComplete();
	}

	void queueFramebufferWarm(std::size_t modelIndex, bool tracked, std::uint64_t generation) {
		if (warmQueuedIndices_.insert(modelIndex).second)
			framebufferWarmQueue_.push_back({modelIndex, 0});
		if (tracked)
			warmTrackedGeneration_[modelIndex] = generation;
	}

	void markConstructionModelComplete(std::size_t modelIndex) {
		if (!constructionCountedIndices_.insert(modelIndex).second)
			return;
		const auto plugin = modelPluginKeyByIndex_.find(modelIndex);
		if (plugin == modelPluginKeyByIndex_.end())
			return;
		auto remaining = constructionPluginRemaining_.find(plugin->second);
		if (remaining != constructionPluginRemaining_.end() && remaining->second > 0 && --remaining->second == 0)
			constructionPluginCompleted_++;
	}

	void resetFramebufferPluginProgress() {
		framebufferPluginModelCounts_.clear();
		for (std::size_t modelIndex = 0; modelIndex < displayEligibility_.size(); ++modelIndex) {
			if (!isDisplayEligible(modelIndex))
				continue;
			const auto plugin = modelPluginKeyByIndex_.find(modelIndex);
			if (plugin != modelPluginKeyByIndex_.end())
				framebufferPluginModelCounts_[plugin->second]++;
		}
		framebufferPluginRemaining_ = framebufferPluginModelCounts_;
		framebufferCountedIndices_.clear();
		framebufferTarget_ = static_cast<int>(framebufferPluginRemaining_.size());
		framebufferCompleted_ = 0;
		for (std::size_t modelIndex = 0; modelIndex < displayEligibility_.size(); ++modelIndex) {
			if (!isDisplayEligible(modelIndex))
				continue;
			DeepcacheModelBox* box = browser_ ? browser_->getModelBox(modelIndex) : nullptr;
			if (box && box->hasValidFramebufferImage())
				markFramebufferModelComplete(modelIndex);
		}
	}

	void markFramebufferModelComplete(std::size_t modelIndex) {
		if (!isDisplayEligible(modelIndex))
			return;
		if (!framebufferCountedIndices_.insert(modelIndex).second)
			return;
		const auto plugin = modelPluginKeyByIndex_.find(modelIndex);
		if (plugin == modelPluginKeyByIndex_.end())
			return;
		auto remaining = framebufferPluginRemaining_.find(plugin->second);
		if (remaining != framebufferPluginRemaining_.end() && remaining->second > 0 && --remaining->second == 0)
			framebufferCompleted_++;
	}

	DeepcacheModule* module_ = nullptr;
	DeepcacheBrowser* browser_ = nullptr;
	deepcache::PreviewPlannerWorker worker_;
	deepcache::MemoryPreviewCacheBackend backend_;
	deepcache::DeepcacheArchiveWorker archive_;
	deepcache::CacheState state_ = deepcache::CacheState::IDLE;
	std::uint64_t nextGeneration_ = 0;
	std::uint64_t activeGeneration_ = 0;
	int completed_ = 0;
	int total_ = 0;
	int failed_ = 0;
	int constructionTarget_ = 0;
	int constructionCompleted_ = 0;
	int constructionFailed_ = 0;
	int constructionPluginTarget_ = 0;
	int constructionPluginCompleted_ = 0;
	int framebufferTarget_ = 0;
	int framebufferCompleted_ = 0;
	std::deque<std::pair<std::size_t, int>> framebufferWarmQueue_;
	std::unordered_set<std::size_t> warmQueuedIndices_;
	std::unordered_map<std::size_t, std::uint64_t> warmTrackedGeneration_;
	std::deque<std::size_t> onDemandBuildQueue_;
	std::unordered_set<std::size_t> onDemandQueuedIndices_;
	std::unordered_set<std::size_t> generationResidentIndices_;
	std::unordered_set<std::size_t> persistentModelIndices_;
	std::unordered_set<std::size_t> compressedModelIndices_;
	std::unordered_set<std::size_t> rehydrationPendingIndices_;
	std::unordered_set<std::size_t> restoreDecodeRequestedIndices_;
	std::unordered_set<std::size_t> restoreUploadQueuedIndices_;
	std::vector<std::uint8_t> displayEligibility_;
	std::vector<std::uint8_t> displayEligibilityScratch_;
	std::deque<std::pair<std::size_t, int>> persistentUploadQueue_;
	std::deque<deepcache::PreviewWrite> archiveWriteRetryQueue_;
	std::size_t pendingUploadBytes_ = 0;
	std::unordered_map<std::string, std::size_t> modelIndexByCacheKey_;
	std::unordered_map<std::size_t, std::string> cacheKeyByModelIndex_;
	std::unordered_map<std::size_t, std::string> fingerprintByModelIndex_;
	std::unordered_map<std::size_t, std::string> modelPluginKeyByIndex_;
	std::unordered_map<std::string, int> constructionPluginRemaining_;
	std::unordered_set<std::size_t> constructionCountedIndices_;
	std::unordered_map<std::string, int> framebufferPluginModelCounts_;
	std::unordered_map<std::string, int> framebufferPluginRemaining_;
	std::unordered_set<std::size_t> framebufferCountedIndices_;
	double warmingStartedAt_ = 0.0;
	double constructionTotalMs_ = 0.0;
	double constructionMaxMs_ = 0.0;
	int constructedCount_ = 0;
	bool stopped_ = false;
	bool archiveStarted_ = false;
	bool startRequested_ = false;
	bool ignoreArchiveResults_ = false;
	bool graphicsContextLost_ = false;
	bool graphicsRestoreScheduled_ = false;
	std::uint64_t graphicsGeneration_ = 0;
	bool lastPreferDarkPanels_ = false;
	int lastPreviewCacheResolutionPercent_ = 100;
	double lastEligibilityCheckAt_ = -INFINITY;
	std::shared_ptr<int> lifetimeToken_ = std::make_shared<int>(0);
};

struct DeepcacheWarmRenderHost : widget::TransparentWidget {
	PreviewCacheManager* cacheManager = nullptr;

	void step() override {
		if (parent)
			box = parent->box.zeroPos();
		widget::TransparentWidget::step();
	}

	void draw(const DrawArgs& args) override {
		(void)args;
		if (cacheManager)
			cacheManager->warmFramebuffers();
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		if (cacheManager)
			cacheManager->onGraphicsContextDestroy();
		widget::TransparentWidget::onContextDestroy(e);
	}
};

struct DeepcacheBrowserDragon : widget::TransparentWidget {
	DeepcacheBrowser* browser = nullptr;
	bool enabled = true;

	void step() override {
		if (!enabled || !browser || !browser->parent) {
			hide();
			return;
		}
		const float freeHeaderWidth = browser->box.size.x - browser->headerContentWidth() - 20.f;
		if (freeHeaderWidth < 140.f) {
			hide();
			return;
		}
		show();
		constexpr float cropWidth = 325.f;
		constexpr float cropHeight = 63.f;
		const float drawWidth = std::min(280.f, freeHeaderWidth);
		const float drawHeight = drawWidth * cropHeight / cropWidth;
		box.size = math::Vec(drawWidth, drawHeight);
		box.pos.x = browser->box.pos.x + browser->box.size.x - 12.f - drawWidth;
		// Keep the body clear of Rack's top bar while the legs and claws cross the browser frame.
		box.pos.y = browser->box.pos.y - drawHeight * 0.35f;
		widget::TransparentWidget::step();
	}

	void draw(const DrawArgs& args) override {
		if (!browser || !browser->ensureDragonImage(args.vg) || box.size.x <= 0.f)
			return;
		constexpr float sourceWidth = 338.f;
		constexpr float sourceHeight = 113.f;
		constexpr float cropX = 6.f;
		constexpr float cropY = 24.f;
		constexpr float cropWidth = 325.f;
		const float scale = box.size.x / cropWidth;
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillPaint(args.vg, nvgImagePattern(args.vg, -cropX * scale, -cropY * scale,
		                                      sourceWidth * scale, sourceHeight * scale,
		                                      0.f, browser->dragonImageHandle, 0.82f));
		nvgFill(args.vg);
	}
};

struct DeepcacheBrowserOverlay : ui::MenuOverlay {
	app::Scene* ownerScene = nullptr;
	widget::Widget* previousBrowser = nullptr;
	DeepcacheBrowser* browser = nullptr;
	DeepcacheBrowserDragon* dragon = nullptr;
	bool installed = false;
	bool retired = false;
	bool ownershipConflict = false;

	explicit DeepcacheBrowserOverlay(PreviewCacheManager* cacheManager, app::Scene* scene)
		: ownerScene(scene) {
		bgColor = nvgRGBAf(0.f, 0.f, 0.f, 0.33f);
		if (!ownerScene)
			return;
		previousBrowser = ownerScene->browser;
		if (previousBrowser) {
			previousBrowser->hide();
			if (previousBrowser->parent == ownerScene)
				ownerScene->removeChild(previousBrowser);
			releaseFramebuffers(previousBrowser);
		}
		auto rollback = [this]() {
			if (!ownerScene)
				return;
			if (ownerScene->browser == this)
				ownerScene->browser = previousBrowser;
			if (parent == ownerScene)
				ownerScene->removeChild(this);
			if (browser) {
				releaseFramebuffers(browser);
				if (dragon) {
					if (dragon->parent == this)
						removeChild(dragon);
					delete dragon;
					dragon = nullptr;
				}
				if (browser->parent == this)
					removeChild(browser);
				delete browser;
				browser = nullptr;
			}
			if (previousBrowser && !previousBrowser->parent) {
				ownerScene->addChild(previousBrowser);
				previousBrowser->hide();
			}
		};
		try {
			browser = new DeepcacheBrowser(cacheManager);
			addChild(browser);
			dragon = new DeepcacheBrowserDragon;
			dragon->browser = browser;
			addChild(dragon);
			ownerScene->browser = this;
			ownerScene->addChild(this);
			hide();
			installed = true;
		}
		catch (const std::exception& exception) {
			WARN("Leviathan Deepcache: browser installation failed: %s", exception.what());
			rollback();
		}
		catch (...) {
			WARN("Leviathan Deepcache: browser installation failed: unknown exception");
			rollback();
		}
	}

	bool ownsBrowserSlot() const {
		return installed && ownerScene && ownerScene->browser == this;
	}

	void restore() {
		if (!ownsBrowserSlot()) {
			ownershipConflict = installed;
			return;
		}
		ownerScene->browser = previousBrowser;
		if (previousBrowser && !previousBrowser->parent) {
			ownerScene->addChild(previousBrowser);
			previousBrowser->hide();
		}
		if (parent == ownerScene)
			ownerScene->removeChild(this);
		installed = false;
	}

	void retireForSuccessor() {
		ownershipConflict = true;
		retired = true;
		if (dragon) {
			if (dragon->parent == this)
				removeChild(dragon);
			delete dragon;
			dragon = nullptr;
		}
		if (browser) {
			releaseFramebuffers(browser);
			removeChild(browser);
			delete browser;
			browser = nullptr;
		}
	}

	void step() override {
		// If a later browser replacement restores this retired link, immediately
		// heal the chain back to the browser that preceded Deepcache. The tiny
		// detached shell is intentionally retained because the successor owns a
		// raw pointer to it and Rack provides no chain notification mechanism.
		if (retired && ownerScene && ownerScene->browser == this) {
			ownerScene->browser = previousBrowser;
			if (previousBrowser && !previousBrowser->parent) {
				ownerScene->addChild(previousBrowser);
				previousBrowser->hide();
			}
			if (parent == ownerScene)
				ownerScene->removeChild(this);
			return;
		}
		if (!parent)
			return;
		box = parent->box.zeroPos();
		if (isVisible())
			Widget::step();
	}

	void onAction(const ActionEvent& e) override {
		hide();
	}

	void onShow(const ShowEvent& e) override {
		if (browser)
			browser->refresh();
		ui::MenuOverlay::onShow(e);
	}
};

DeepcacheModelBox::DeepcacheModelBox(plugin::Model* model, std::size_t modelIndex,
	                                 int pluginModelOrder, PreviewCacheManager* manager)
	: model(model), modelIndex(modelIndex), pluginModelOrder(pluginModelOrder), cacheManager(manager) {
	updateZoom();
}

DeepcacheModelBox::~DeepcacheModelBox() {
	setTooltip(nullptr);
	clearPreview();
}

void DeepcacheModelBox::updateZoom() {
	const float zoom = std::pow(2.f, settings::browserZoom);
	if (previewRoot && zoomWidget && moduleWidget) {
		framebuffer->setDirty();
		zoomWidget->setZoom(zoom);
		box.size.x = moduleWidget->box.size.x * zoom;
	}
	else if (rasterWidget && rasterWidget->width > 0 && rasterWidget->height > 0) {
		box.size.x = RACK_GRID_HEIGHT * (rasterWidget->width / static_cast<float>(rasterWidget->height)) * zoom;
	}
	else {
		box.size.x = 12.f * RACK_GRID_WIDTH * zoom;
	}
	box.size.y = RACK_GRID_HEIGHT * zoom;
	box.size = box.size.ceil();
	if (rasterWidget)
		rasterWidget->box.size = box.size;
}

bool DeepcacheModelBox::installDecodedPreview(deepcache::DecodedPreview preview) {
	if (preview.rgba.empty() || preview.width <= 0 || preview.height <= 0 ||
	    state == deepcache::PreviewEntryState::CONSTRUCTING)
		return false;
	auto pixels = std::make_shared<const std::vector<std::uint8_t>>(std::move(preview.rgba));
	return installRasterPreview(std::move(pixels), preview.width, preview.height);
}

bool DeepcacheModelBox::installRasterPreview(std::shared_ptr<const std::vector<std::uint8_t>> rgba,
	                                         int width, int height) {
	if (!rgba || rgba->empty() || width <= 0 || height <= 0 ||
	    state == deepcache::PreviewEntryState::CONSTRUCTING)
		return false;
	clearPreview();
	rasterWidget = new DeepcacheRasterWidget;
	rasterWidget->rgba = std::move(rgba);
	rasterWidget->width = width;
	rasterWidget->height = height;
	addChild(rasterWidget);
	state = deepcache::PreviewEntryState::RESIDENT;
	updateZoom();
	return true;
}

bool DeepcacheModelBox::ensurePersistentImage(NVGcontext* vg) {
	if (!rasterWidget || !rasterWidget->ensureImage(vg))
		return false;
	state = deepcache::PreviewEntryState::FRAMEBUFFER_READY;
	return true;
}

bool DeepcacheModelBox::hasCpuPixels() const {
	return rasterWidget && rasterWidget->hasCpuPixels();
}

std::size_t DeepcacheModelBox::cpuPixelBytes() const {
	return rasterWidget ? rasterWidget->cpuPixelBytes() : 0;
}

std::uint64_t DeepcacheModelBox::estimatedGpuBytes() const {
	if (!rasterWidget || state != deepcache::PreviewEntryState::FRAMEBUFFER_READY ||
	    rasterWidget->width <= 0 || rasterWidget->height <= 0)
		return 0;
	return static_cast<std::uint64_t>(rasterWidget->width) *
	       static_cast<std::uint64_t>(rasterWidget->height) * 4ull;
}

void DeepcacheModelBox::releaseCpuPixels() {
	if (rasterWidget)
		rasterWidget->releaseCpuPixels();
}

void DeepcacheModelBox::releasePersistentImage() {
	if (!rasterWidget)
		return;
	rasterWidget->releaseImage();
	if (state == deepcache::PreviewEntryState::FRAMEBUFFER_READY)
		state = deepcache::PreviewEntryState::RESIDENT;
}

bool DeepcacheModelBox::captureFramebuffer(std::vector<std::uint8_t>& rgba, int& width, int& height) const {
	if (!framebuffer || !hasValidFramebufferImage())
		return false;
	NVGLUframebuffer* fb = framebuffer->getFramebuffer();
	if (!fb || fb->texture == 0)
		return false;
	const math::Vec size = framebuffer->getFramebufferSize();
	width = static_cast<int>(std::round(size.x));
	height = static_cast<int>(std::round(size.y));
	if (width <= 0 || height <= 0 || width > 16384 || height > 16384 ||
	    static_cast<std::uint64_t>(width) * height * 4ull > 128ull * 1024ull * 1024ull)
		return false;
	rgba.resize(static_cast<std::size_t>(width) * height * 4u);
	GLint previousTexture = 0;
	GLint previousAlignment = 4;
	while (glGetError() != GL_NO_ERROR) {}
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
	glGetIntegerv(GL_PACK_ALIGNMENT, &previousAlignment);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, fb->texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture));
	glPixelStorei(GL_PACK_ALIGNMENT, previousAlignment);
	if (glGetError() != GL_NO_ERROR) {
		rgba.clear();
		return false;
	}
	const std::size_t stride = static_cast<std::size_t>(width) * 4u;
	std::vector<std::uint8_t> row(stride);
	for (int y = 0; y < height / 2; ++y) {
		std::uint8_t* top = rgba.data() + static_cast<std::size_t>(y) * stride;
		std::uint8_t* bottom = rgba.data() + static_cast<std::size_t>(height - 1 - y) * stride;
		std::memcpy(row.data(), top, stride);
		std::memcpy(top, bottom, stride);
		std::memcpy(bottom, row.data(), stride);
	}
	return true;
}

bool DeepcacheModelBox::ensurePreviewConstructed() {
	if (state == deepcache::PreviewEntryState::RESIDENT ||
	    state == deepcache::PreviewEntryState::FRAMEBUFFER_READY)
		return true;
	if (state == deepcache::PreviewEntryState::CONSTRUCTING ||
	    state == deepcache::PreviewEntryState::FAILED)
		return false;
	state = deepcache::PreviewEntryState::CONSTRUCTING;

	try {
		previewRoot = new widget::TransparentWidget;
		addChild(previewRoot);
		zoomWidget = new widget::ZoomWidget;
		previewRoot->addChild(zoomWidget);
		framebuffer = new widget::FramebufferWidget;
		if (APP && APP->window && APP->window->pixelRatio < 2.f)
			framebuffer->oversample = 2.f;
		framebuffer->dirtyOnSubpixelChange = false;
		zoomWidget->addChild(framebuffer);
		moduleContainer = new ModuleWidgetContainer;
		framebuffer->addChild(moduleContainer);
		moduleWidget = model ? model->createModuleWidget(nullptr) : nullptr;
		if (!moduleWidget)
			throw std::runtime_error("createModuleWidget(nullptr) returned null");
		moduleContainer->addChild(moduleWidget);
		moduleContainer->box.size = moduleWidget->box.size;
		framebuffer->box.size = moduleWidget->box.size;
		moduleWidget->step();
		updateZoom();
		state = deepcache::PreviewEntryState::RESIDENT;
		failureReason.clear();
		return true;
	}
	catch (const std::exception& exception) {
		failureReason = exception.what();
	}
	catch (...) {
		failureReason = "unknown exception";
	}

	if (isDragonKingDebugEnabled()) {
		WARN("Leviathan Deepcache: preview failed: %s: %s",
		     model ? model->getFullName().c_str() : "unknown", failureReason.c_str());
	}
	const std::string capturedFailure = failureReason;
	state = deepcache::PreviewEntryState::EMPTY;
	clearPreview();
	failureReason = capturedFailure;
	state = deepcache::PreviewEntryState::FAILED;
	return false;
}

bool DeepcacheModelBox::hasValidFramebufferImage() const {
	NVGcontext* currentVg = APP && APP->window ? APP->window->vg : nullptr;
	if (rasterWidget && currentVg && rasterWidget->ownerVg == currentVg && rasterWidget->imageHandle >= 0)
		return nvg_gfx_lifecycle::ownedNvgImageSizeMatches(currentVg,
		                                                       rasterWidget->imageHandle,
		                                                       rasterWidget->width,
		                                                       rasterWidget->height);
	if (!framebuffer || !APP || !APP->window || !APP->window->vg)
		return false;
	const int imageHandle = framebuffer->getImageHandle();
	if (imageHandle < 0)
		return false;
	int imageWidth = 0;
	int imageHeight = 0;
	nvgImageSize(APP->window->vg, imageHandle, &imageWidth, &imageHeight);
	return imageWidth > 0 && imageHeight > 0;
}

FramebufferWarmResult DeepcacheModelBox::warmFramebuffer() {
	if (state == deepcache::PreviewEntryState::FRAMEBUFFER_READY && hasValidFramebufferImage())
		return FramebufferWarmResult::READY;
	if (state == deepcache::PreviewEntryState::RESIDENT && rasterWidget) {
		if (!APP || !APP->window || !APP->window->vg)
			return FramebufferWarmResult::RETRY;
		return ensurePersistentImage(APP->window->vg) ? FramebufferWarmResult::READY
		                                              : FramebufferWarmResult::RETRY;
	}
	if (state != deepcache::PreviewEntryState::RESIDENT || !framebuffer)
		return FramebufferWarmResult::FAILED;
	if (!APP || !APP->window || !APP->window->vg || !APP->window->fbVg)
		return FramebufferWarmResult::RETRY;
	if (moduleWidget) {
		// Fractal glass is produced off-thread. Re-step the preview tree while
		// warming and do not persist a framebuffer until every overlay has
		// published its pixels (or reported that no fallback exists).
		moduleWidget->step();
		if (!fractalOverlaysReadyForCapture(moduleWidget)) {
			return FramebufferWarmResult::PENDING_ASSET;
		}
	}
	try {
		// Rack can lazily render this framebuffer while the browser is open, in
		// which case its resolution follows the current browser transform. Always
		// replace that image with Deepcache's canonical render before capture so
		// persistence does not depend on browser timing or zoom.
		if (hasValidFramebufferImage())
			framebuffer->deleteFramebuffer();
		// Match Rack's own policy at the current UI scale. A preview constructed
		// before a scale change must not retain an unnecessarily expensive 2x
		// temporary oversample after the window has moved to HiDPI rendering.
		framebuffer->oversample = APP->window->pixelRatio < 2.f ? 2.f : 1.f;
		framebuffer->step();
		framebuffer->setDirty();
		// Called only by the scene-level warm host during Rack's draw phase.
		// render() builds the texture without compositing it into the scene.
		const float canonicalScale = cacheManager && cacheManager->previewCacheResolutionPercent() == 200 ? 2.f : 1.f;
		const float renderScale = deepcache::previewRenderTransformScale(APP->window->pixelRatio, canonicalScale);
		framebuffer->render(math::Vec(renderScale, renderScale));
		if (!hasValidFramebufferImage())
			return FramebufferWarmResult::RETRY;
		state = deepcache::PreviewEntryState::FRAMEBUFFER_READY;
		return FramebufferWarmResult::READY;
	}
	catch (const std::exception& exception) {
		if (isDragonKingDebugEnabled()) {
			WARN("Leviathan Deepcache: framebuffer warm failed: %s: %s",
			     model ? model->getFullName().c_str() : "unknown", exception.what());
		}
	}
	catch (...) {
		if (isDragonKingDebugEnabled()) {
			WARN("Leviathan Deepcache: framebuffer warm failed: %s: unknown exception",
			     model ? model->getFullName().c_str() : "unknown");
		}
	}
	return FramebufferWarmResult::FAILED;
}

void DeepcacheModelBox::invalidateFramebufferReadiness() {
	if (state == deepcache::PreviewEntryState::FRAMEBUFFER_READY)
		state = deepcache::PreviewEntryState::RESIDENT;
}

void DeepcacheModelBox::clearPreview() {
	if (state == deepcache::PreviewEntryState::CONSTRUCTING)
		return;
	if (previewRoot) {
		releaseFramebuffers(previewRoot);
		if (previewRoot->parent == this)
			removeChild(previewRoot);
		delete previewRoot;
	}
	if (rasterWidget) {
		if (rasterWidget->parent == this)
			removeChild(rasterWidget);
		delete rasterWidget;
	}
	previewRoot = nullptr;
	zoomWidget = nullptr;
	framebuffer = nullptr;
	moduleWidget = nullptr;
	moduleContainer = nullptr;
	rasterWidget = nullptr;
	failureReason.clear();
	state = deepcache::PreviewEntryState::EMPTY;
	updateZoom();
}

void DeepcacheModelBox::draw(const DrawArgs& args) {
	if (state == deepcache::PreviewEntryState::EMPTY || state == deepcache::PreviewEntryState::QUEUED) {
		if (cacheManager)
			cacheManager->requestOnDemand(modelIndex);
	}

	nvgBeginPath(args.vg);
	const float radius = 10.f;
	const math::Rect shadowBox = box.zeroPos().grow(math::Vec(radius, radius));
	nvgRect(args.vg, RECT_ARGS(shadowBox));
	nvgFillPaint(args.vg, nvgBoxGradient(args.vg, 0, 0, box.size.x, box.size.y, 5.f, radius,
	                                 nvgRGBAf(0, 0, 0, 0.5f), color::BLACK_TRANSPARENT));
	nvgFill(args.vg);

	if (state == deepcache::PreviewEntryState::FAILED) {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(35, 10, 18, 255));
		nvgFill(args.vg);
		nvgFontSize(args.vg, 8.f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, color::WHITE);
		nvgTextBox(args.vg, 8.f, box.size.y * 0.45f, box.size.x - 16.f,
		           model ? model->name.c_str() : "Preview failed", nullptr);
	}

	const float brightness = math::clamp(settings::rackBrightness + 0.2f, 0.f, 1.f);
	nvgGlobalTint(args.vg, nvgRGBAf(brightness, brightness, brightness, 1.f));
	OpaqueWidget::draw(args);

	if (model && model->isFavorite()) {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, RECT_ARGS(box.zeroPos()));
		nvgStrokeWidth(args.vg, 2.f);
		nvgStrokeColor(args.vg, componentlibrary::SCHEME_YELLOW);
		nvgStroke(args.vg);
	}
}

void DeepcacheModelBox::onButton(const ButtonEvent& e) {
	if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0) {
		if (auto* browser = getAncestorOfType<DeepcacheBrowser>()) {
			if (app::ModuleWidget* addedWidget = browser->chooseModel(model)) {
				// Keep ownership of the initiating click. Passing it directly to the
				// module makes its drag offset relative to this browser card, moving it
				// away from the rack position where the browser was opened.
				pendingAddedWidget = addedWidget;
				pendingDragDelta = math::Vec();
				e.consume(this);
			}
			else {
				e.consume(this);
			}
		}
	}
	else if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT &&
	         (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
		model->setFavorite(!model->isFavorite());
		if (auto* browser = getAncestorOfType<DeepcacheBrowser>())
			browser->refresh();
		e.consume(this);
	}
	else if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
		createContextMenu();
		e.consume(this);
	}
	if (!e.isConsumed())
		OpaqueWidget::onButton(e);
}

void DeepcacheModelBox::onDragMove(const DragMoveEvent& e) {
	if (!pendingAddedWidget)
		return;

	pendingDragDelta = pendingDragDelta.plus(e.mouseDelta);
	constexpr float transferDistance = 4.f;
	if (pendingDragDelta.x * pendingDragDelta.x + pendingDragDelta.y * pendingDragDelta.y <
	    transferDistance * transferDistance)
		return;

	if (!APP || !APP->event || !APP->scene || !APP->scene->rack || !pendingAddedWidget->parent) {
		pendingAddedWidget = nullptr;
		pendingDragDelta = math::Vec();
		return;
	}

	app::ModuleWidget* addedWidget = pendingAddedWidget;
	pendingAddedWidget = nullptr;
	pendingDragDelta = math::Vec();
	app::RackWidget* rack = APP->scene->rack;
	rack->setModulePosNearest(addedWidget, rack->getMousePos().minus(addedWidget->box.size.div(2.f)));
	APP->event->setDraggedWidget(addedWidget, e.button);
}

void DeepcacheModelBox::onDragEnd(const DragEndEvent& e) {
	pendingAddedWidget = nullptr;
	pendingDragDelta = math::Vec();
	OpaqueWidget::onDragEnd(e);
}

void DeepcacheModelBox::setTooltip(ui::Tooltip* nextTooltip) {
	if (tooltip) {
		tooltip->requestDelete();
	}
	tooltip = nextTooltip;
	if (tooltip && APP && APP->scene)
		APP->scene->addChild(tooltip);
}

void DeepcacheModelBox::onEnter(const EnterEvent& e) {
	auto* nextTooltip = new ui::Tooltip;
	nextTooltip->text = model->name + "\n" + model->plugin->brand;
	if (!model->description.empty())
		nextTooltip->text += "\n" + model->description;
	if (state == deepcache::PreviewEntryState::FAILED)
		nextTooltip->text += "\n\nDeepcache preview failed: " + failureReason;
	setTooltip(nextTooltip);
	OpaqueWidget::onEnter(e);
}

void DeepcacheModelBox::onLeave(const LeaveEvent& e) {
	setTooltip(nullptr);
	OpaqueWidget::onLeave(e);
}

void DeepcacheModelBox::onHide(const HideEvent& e) {
	setTooltip(nullptr);
	OpaqueWidget::onHide(e);
}

void DeepcacheModelBox::createContextMenu() {
	ui::Menu* menu = createMenu();
	menu->addChild(createMenuLabel(model->name));
	menu->addChild(createMenuLabel(model->plugin->brand));
	model->appendContextMenu(menu, true);
	if (state == deepcache::PreviewEntryState::FAILED) {
		menu->addChild(new ui::MenuSeparator);
		PreviewCacheManager* manager = cacheManager;
		const std::weak_ptr<int> lifetime = manager ? manager->lifetimeToken() : std::weak_ptr<int>();
		const std::size_t retryIndex = modelIndex;
		menu->addChild(createMenuItem("Retry Deepcache preview", "", [manager, lifetime, retryIndex]() {
			if (!lifetime.expired())
				manager->retryPreview(retryIndex);
		}));
	}
}

DeepcacheBrowser::DeepcacheBrowser(PreviewCacheManager* manager)
	: cacheManager(manager) {
	const float margin = 10.f;
	headerLayout = new ui::SequentialLayout;
	headerLayout->margin = math::Vec(margin, margin);
	headerLayout->spacing = math::Vec(margin, margin);
	addChild(headerLayout);

	searchField = new DeepcacheBrowserSearchField;
	searchField->browser = this;
	searchField->box.size.x = 150.f;
	searchField->placeholder = string::translate("Browser.searchModules");
	headerLayout->addChild(searchField);

	brandButton = new DeepcacheBrandButton;
	brandButton->browser = this;
	brandButton->box.size.x = 150.f;
	headerLayout->addChild(brandButton);

	tagButton = new DeepcacheTagButton;
	tagButton->browser = this;
	tagButton->box.size.x = 150.f;
	headerLayout->addChild(tagButton);

	auto* slugButton = new DeepcacheSlugButton;
	slugButton->box.size.x = 70.f;
	headerLayout->addChild(slugButton);

	favoriteQuantity = new DeepcacheFavoriteQuantity;
	favoriteQuantity->browser = this;
	favoriteButton = new ui::OptionButton;
	favoriteButton->quantity = favoriteQuantity;
	favoriteButton->text = string::translate("Browser.favorites");
	favoriteButton->box.size.x = 70.f;
	headerLayout->addChild(favoriteButton);

	auto* clearButton = new DeepcacheClearButton;
	clearButton->browser = this;
	clearButton->text = string::translate("Browser.resetFilters");
	clearButton->box.size.x = 110.f;
	headerLayout->addChild(clearButton);

	countLabel = new ui::Label;
	countLabel->box.size.x = 110.f;
	headerLayout->addChild(countLabel);

	auto* sortButton = new DeepcacheSortButton;
	sortButton->browser = this;
	sortButton->box.size.x = 150.f;
	headerLayout->addChild(sortButton);

	auto* zoomButton = new DeepcacheZoomButton;
	zoomButton->browser = this;
	zoomButton->box.size.x = 100.f;
	headerLayout->addChild(zoomButton);

	auto* libraryButton = new DeepcacheUrlButton;
	libraryButton->text = string::translate("Browser.browseLibrary");
	libraryButton->url = "https://library.vcvrack.com/";
	libraryButton->box.size.x = 150.f;
	headerLayout->addChild(libraryButton);

	modelScroll = new ui::ScrollWidget;
	addChild(modelScroll);
	modelMargin = new widget::Widget;
	modelScroll->container->addChild(modelMargin);
	modelContainer = new ui::SequentialLayout;
	modelContainer->margin = math::Vec(margin, 2.f);
	modelContainer->spacing = math::Vec(margin, margin);
	modelMargin->addChild(modelContainer);

	struct ModelSortRecord {
		plugin::Model* model = nullptr;
		int pluginModelOrder = 0;
		std::string brand;
		std::string name;
		std::string slug;
		ModelSortRecord(plugin::Model* model, int pluginModelOrder)
			: model(model),
			  pluginModelOrder(pluginModelOrder),
			  brand(lowercase(model->plugin->brand)),
			  name(lowercase(model->name)),
			  slug(lowercase(model->slug)) {
		}
	};
	std::vector<ModelSortRecord> models;
	for (plugin::Plugin* plugin : plugin::plugins) {
		int pluginModelOrder = 0;
		for (plugin::Model* model : plugin->models)
			models.emplace_back(model, pluginModelOrder++);
	}
	std::stable_sort(models.begin(), models.end(), [](const ModelSortRecord& a, const ModelSortRecord& b) {
		return std::tie(a.brand, a.name, a.slug) < std::tie(b.brand, b.name, b.slug);
	});

	modelBoxes.reserve(models.size());
	modelDescriptors.reserve(models.size());
	browserRecords.reserve(models.size());
	std::unordered_map<const plugin::Plugin*, std::string> pluginFingerprints;
	const int previewResolutionPercent = manager ? manager->previewCacheResolutionPercent() : 100;
	for (std::size_t index = 0; index < models.size(); ++index) {
		plugin::Model* model = models[index].model;
		auto fingerprint = pluginFingerprints.find(model->plugin);
		if (fingerprint == pluginFingerprints.end())
			fingerprint = pluginFingerprints.emplace(
				model->plugin, pluginArtifactFingerprint(model->plugin, previewResolutionPercent)).first;
		auto* modelBox = new DeepcacheModelBox(model, index, models[index].pluginModelOrder, manager);
		modelBoxes.push_back(modelBox);
		modelContainer->addChild(modelBox);
		modelDescriptors.push_back({index, model->plugin->slug, model->plugin->version, model->slug,
		                            model->plugin->brand, model->name, model->isFavorite(), model->hidden,
		                            fingerprint->second});
		browserRecords.push_back(makeBrowserRecord(modelBox));
	}
	lastBrowserZoom = settings::browserZoom;
	lastPreferDarkPanels = settings::preferDarkPanels;
	refresh();
}

DeepcacheBrowser::~DeepcacheBrowser() {
	lifetimeToken.reset();
	NVGcontext* current = APP && APP->window ? APP->window->vg : nullptr;
	nvg_gfx_lifecycle::resetOwnedNvgImage(dragonOwnerVg, dragonImageHandle,
	                                     dragonImageWidth, dragonImageHeight,
	                                     current, dragonOwnerVg == current);
	delete favoriteQuantity;
}

void DeepcacheBrowser::step() {
	box = parent->box.zeroPos().grow(math::Vec(-40.f, -40.f));
	headerLayout->box.size.x = box.size.x;
	modelScroll->box.pos = headerLayout->box.getBottomLeft();
	modelScroll->box.size = box.size.minus(modelScroll->box.pos);
	modelMargin->box.size.x = modelScroll->box.size.x;
	modelMargin->box.size.y = modelContainer->box.size.y + 10.f;
	modelContainer->box.size.x = modelMargin->box.size.x - 10.f;

	if (settings::browserZoom != lastBrowserZoom) {
		lastBrowserZoom = settings::browserZoom;
		updateZoom();
	}
	if (settings::preferDarkPanels != lastPreferDarkPanels) {
		lastPreferDarkPanels = settings::preferDarkPanels;
		Widget::DirtyEvent dirty;
		modelContainer->onDirty(dirty);
	}
	OpaqueWidget::step();
}

void DeepcacheBrowser::draw(const DrawArgs& args) {
	// Keep the browser dark enough for arbitrary module panels while carrying
	// Leviathan's violet branding across the full surface.
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
	const NVGpaint background = nvgLinearGradient(
		args.vg, 0.f, 0.f, box.size.x, box.size.y,
		nvgRGBA(31, 25, 44, 255), nvgRGBA(15, 13, 23, 255));
	nvgFillPaint(args.vg, background);
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.5f, 0.5f, std::max(0.f, box.size.x - 1.f), std::max(0.f, box.size.y - 1.f));
	nvgStrokeWidth(args.vg, 1.f);
	nvgStrokeColor(args.vg, nvgRGBA(117, 91, 190, 68));
	nvgStroke(args.vg);
	Widget::draw(args);
}

bool DeepcacheBrowser::ensureDragonImage(NVGcontext* vg) {
	if (!vg)
		return false;
	if (dragonOwnerVg == vg && dragonImageHandle >= 0 && dragonImageWidth > 0 && dragonImageHeight > 0 &&
	    nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg, dragonImageHandle,
	                                                dragonImageWidth, dragonImageHeight))
		return true;

	nvg_gfx_lifecycle::resetOwnedNvgImage(dragonOwnerVg, dragonImageHandle,
	                                     dragonImageWidth, dragonImageHeight,
	                                     vg, dragonOwnerVg == vg);
	const std::string path = asset::plugin(pluginInstance, "res/icon/Leviathan_DrHSmall.png");
	dragonImageHandle = nvgCreateImage(vg, path.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
	if (dragonImageHandle < 0)
		return false;
	dragonOwnerVg = vg;
	nvgImageSize(vg, dragonImageHandle, &dragonImageWidth, &dragonImageHeight);
	if (dragonImageWidth <= 0 || dragonImageHeight <= 0) {
		nvg_gfx_lifecycle::resetOwnedNvgImage(dragonOwnerVg, dragonImageHandle,
		                                     dragonImageWidth, dragonImageHeight, vg, true);
		return false;
	}
	return true;
}

float DeepcacheBrowser::headerContentWidth() const {
	if (!headerLayout)
		return 0.f;
	float width = 2.f * headerLayout->margin.x;
	bool hasVisibleChild = false;
	for (Widget* child : headerLayout->children) {
		if (!child || !child->isVisible())
			continue;
		if (hasVisibleChild)
			width += headerLayout->spacing.x;
		width += child->box.size.x;
		hasVisibleChild = true;
	}
	return width;
}

void DeepcacheBrowser::onContextDestroy(const ContextDestroyEvent& e) {
	nvg_gfx_lifecycle::resetOwnedNvgImage(dragonOwnerVg, dragonImageHandle,
	                                     dragonImageWidth, dragonImageHeight,
	                                     nullptr, false);
	widget::OpaqueWidget::onContextDestroy(e);
}

void DeepcacheBrowser::onButton(const ButtonEvent& e) {
	Widget::onButton(e);
	e.stopPropagating();
	if (!e.isConsumed())
		e.consume(this);
}

void DeepcacheBrowser::refresh() {
	modelScroll->offset = math::Vec();
	deepcache::BrowserFilter filter;
	filter.search = string::trim(search);
	filter.brand = brand;
	filter.tagIds = tagIds;
	filter.favoritesOnly = favoritesOnly;
	deepcache::normalizeBrowserFilter(filter);
	int visibleCount = 0;
	std::unordered_set<std::size_t> visibleIndices;
	visibleIndices.reserve(modelBoxes.size());
	for (DeepcacheModelBox* box : modelBoxes) {
		updateBrowserRecord(box);
		const bool visible = deepcache::browserModelMatches(browserRecords[box->modelIndex], filter);
		box->setVisible(visible);
		if (visible) {
			visibleCount++;
			visibleIndices.insert(box->modelIndex);
		}
	}
	if (cacheManager)
		cacheManager->promote(visibleIndices);
	sortModels();
	countLabel->text = std::to_string(visibleCount) + (visibleCount == 1 ? " module" : " modules");
}

void DeepcacheBrowser::clearFilters() {
	search.clear();
	searchField->setText("");
	brand.clear();
	tagIds.clear();
	favoritesOnly = false;
	refresh();
}

deepcache::BrowserModelRecord DeepcacheBrowser::makeBrowserRecord(const DeepcacheModelBox* box) const {
	deepcache::BrowserModelRecord record;
	if (!box || !box->model)
		return record;
	plugin::Model* model = box->model;
	record.modelIndex = box->modelIndex;
	record.pluginSlug = model->plugin->slug;
	record.pluginBrand = model->plugin->brand;
	record.pluginName = model->plugin->name;
	record.modelSlug = model->slug;
	record.modelName = model->name;
	record.description = model->description;
	record.tagIds.assign(model->tagIds.begin(), model->tagIds.end());
	for (int tagId : model->tagIds) {
		if (tagId < 0 || tagId >= static_cast<int>(tag::tagAliases.size()))
			continue;
		for (const std::string& alias : tag::tagAliases[tagId]) {
			record.tagSearchText += alias;
			record.tagSearchText += ' ';
		}
	}
	record.hidden = model->hidden;
	const settings::ModuleInfo* info = settings::getModuleInfo(model->plugin->slug, model->slug);
	record.enabled = !info || info->enabled;
	record.whitelisted = settings::isModuleWhitelisted(model->plugin->slug, model->slug);
	record.favorite = model->isFavorite();
	record.pluginModifiedTimestamp = model->plugin->modifiedTimestamp;
	record.lastAdded = info ? info->lastAdded : -INFINITY;
	record.addedCount = info ? info->added : 0;
	record.pluginModelOrder = box->pluginModelOrder;
	deepcache::normalizeBrowserModelRecord(record);
	return record;
}

void DeepcacheBrowser::updateBrowserRecord(const DeepcacheModelBox* box) {
	if (!box || !box->model || box->modelIndex >= browserRecords.size())
		return;
	plugin::Model* model = box->model;
	deepcache::BrowserModelRecord& record = browserRecords[box->modelIndex];
	record.hidden = model->hidden;
	const settings::ModuleInfo* info = settings::getModuleInfo(model->plugin->slug, model->slug);
	record.enabled = !info || info->enabled;
	record.whitelisted = settings::isModuleWhitelisted(model->plugin->slug, model->slug);
	record.favorite = model->isFavorite();
	record.pluginModifiedTimestamp = model->plugin->modifiedTimestamp;
	record.lastAdded = info ? info->lastAdded : -INFINITY;
	record.addedCount = info ? info->added : 0;
}

void DeepcacheBrowser::sortModels() {
	if (settings::browserSort == settings::BROWSER_SORT_RANDOM && string::trim(search).empty()) {
		for (deepcache::BrowserModelRecord& record : browserRecords)
			record.randomOrder = random::u64();
	}
	const auto indices = deepcache::sortBrowserModelIndices(
		browserRecords, static_cast<deepcache::BrowserSortMode>(settings::browserSort), string::trim(search));
	std::vector<std::size_t> ranks(modelBoxes.size(), modelBoxes.size());
	for (std::size_t rank = 0; rank < indices.size(); ++rank) {
		if (indices[rank] < ranks.size())
			ranks[indices[rank]] = rank;
	}
	modelContainer->children.sort([&](Widget* left, Widget* right) {
		const auto* leftBox = static_cast<DeepcacheModelBox*>(left);
		const auto* rightBox = static_cast<DeepcacheModelBox*>(right);
		return ranks[leftBox->modelIndex] < ranks[rightBox->modelIndex];
	});
}

void DeepcacheBrowser::updateZoom() {
	modelScroll->offset = math::Vec();
	for (DeepcacheModelBox* box : modelBoxes)
		box->updateZoom();
}

void DeepcacheBrowser::clearPreviews() {
	for (DeepcacheModelBox* box : modelBoxes)
		box->clearPreview();
}

DeepcacheModelBox* DeepcacheBrowser::getModelBox(std::size_t index) const {
	return index < modelBoxes.size() ? modelBoxes[index] : nullptr;
}

std::vector<deepcache::ModelDescriptor> DeepcacheBrowser::snapshotModelDescriptors() const {
	std::vector<deepcache::ModelDescriptor> snapshot = modelDescriptors;
	for (deepcache::ModelDescriptor& descriptor : snapshot) {
		DeepcacheModelBox* box = getModelBox(descriptor.modelIndex);
		if (!box || !box->model)
			continue;
		descriptor.favorite = box->model->isFavorite();
		descriptor.hidden = box->model->hidden;
	}
	return snapshot;
}

std::unordered_set<std::size_t> DeepcacheBrowser::visibleModelIndices() const {
	std::unordered_set<std::size_t> indices;
	for (DeepcacheModelBox* box : modelBoxes) {
		if (box->isVisible())
			indices.insert(box->modelIndex);
	}
	return indices;
}

void DeepcacheBrowser::writeDisplayEligibility(std::vector<std::uint8_t>* eligibility) const {
	if (!eligibility)
		return;
	if (eligibility->size() != modelBoxes.size())
		eligibility->resize(modelBoxes.size());
	for (std::size_t index = 0; index < modelBoxes.size(); ++index) {
		DeepcacheModelBox* box = modelBoxes[index];
		(*eligibility)[index] = box && rackModelIsDisplayEligible(box->model) ? 1u : 0u;
	}
}

std::size_t DeepcacheBrowser::residentPreviewCount() const {
	return static_cast<std::size_t>(std::count_if(modelBoxes.begin(), modelBoxes.end(), [](const DeepcacheModelBox* box) {
		return box->state == deepcache::PreviewEntryState::RESIDENT ||
		       box->state == deepcache::PreviewEntryState::FRAMEBUFFER_READY;
	}));
}

std::size_t DeepcacheBrowser::framebufferReadyPreviewCount() const {
	return static_cast<std::size_t>(std::count_if(modelBoxes.begin(), modelBoxes.end(), [](const DeepcacheModelBox* box) {
		return box->state == deepcache::PreviewEntryState::FRAMEBUFFER_READY;
	}));
}

void DeepcacheBrowser::invalidateFramebufferReadiness() {
	for (DeepcacheModelBox* box : modelBoxes)
		box->invalidateFramebufferReadiness();
}

app::ModuleWidget* DeepcacheBrowser::chooseModel(plugin::Model* model) {
	if (!model || !APP || !APP->engine || !APP->scene || !APP->scene->rack)
		return nullptr;
	settings::ModuleInfo& info = settings::moduleInfos[model->plugin->slug][model->slug];
	info.added++;
	info.lastAdded = system::getUnixTime();

	engine::Module* addedModule = nullptr;
	app::ModuleWidget* addedWidget = nullptr;
	try {
		addedModule = model->createModule();
		if (!addedModule)
			throw std::runtime_error("createModule() returned null");
		APP->engine->addModule(addedModule);
		addedWidget = model->createModuleWidget(addedModule);
		if (!addedWidget) {
			APP->engine->removeModule(addedModule);
			delete addedModule;
			addedModule = nullptr;
			throw std::runtime_error("createModuleWidget(module) returned null");
		}

		std::unique_ptr<history::ComplexAction> historyAction(new history::ComplexAction);
		historyAction->name = "add module";
		APP->scene->rack->deselectAll();
		APP->scene->rack->updateModuleOldPositions();
		APP->scene->rack->addModuleAtMouse(addedWidget);
		historyAction->push(APP->scene->rack->getModuleDragAction());
		addedWidget->loadTemplate();
		auto* moduleAdd = new history::ModuleAdd;
		moduleAdd->setModule(addedWidget);
		historyAction->push(moduleAdd);
		APP->history->push(historyAction.release());
		if (APP->scene->browser)
			APP->scene->browser->hide();
		return addedWidget;
	}
	catch (const std::exception& exception) {
		WARN("Leviathan Deepcache: could not add %s: %s", model->getFullName().c_str(), exception.what());
	}
	catch (...) {
		WARN("Leviathan Deepcache: could not add %s: unknown exception", model->getFullName().c_str());
	}
	if (addedWidget) {
		if (addedWidget->parent)
			APP->scene->rack->removeModule(addedWidget);
		delete addedWidget;
	}
	else if (addedModule && APP->engine->hasModule(addedModule)) {
		APP->engine->removeModule(addedModule);
		delete addedModule;
	}
	return nullptr;
}

void DeepcacheBrowserSearchField::step() {
	if (APP && APP->event)
		APP->event->setSelectedWidget(this);
	ui::TextField::step();
}

void DeepcacheBrowserSearchField::onChange(const ChangeEvent& e) {
	browser->search = text;
	browser->refresh();
	ui::TextField::onChange(e);
}

void DeepcacheBrowserSearchField::onAction(const ActionEvent& e) {
	for (Widget* child : browser->modelContainer->children) {
		auto* box = static_cast<DeepcacheModelBox*>(child);
		if (box->isVisible()) {
			browser->chooseModel(box->model);
			break;
		}
	}
}

void DeepcacheBrowserSearchField::onShow(const ShowEvent& e) {
	selectAll();
	ui::TextField::onShow(e);
}

void DeepcacheBrowserSearchField::onHide(const HideEvent& e) {
	if (APP && APP->event)
		APP->event->setSelectedWidget(nullptr);
	ui::TextField::onHide(e);
}

void DeepcacheFavoriteQuantity::setValue(float value) {
	browser->favoritesOnly = value >= 0.5f;
	browser->refresh();
}

float DeepcacheFavoriteQuantity::getValue() {
	return browser->favoritesOnly ? 1.f : 0.f;
}

void DeepcacheMulticolumnMenu::step() {
	// Let Rack establish the natural item heights and menu width first, then
	// replace only an overflowing vertical layout with an adaptive column grid.
	ui::Menu::step();
	if (!APP || !APP->scene || children.empty())
		return;

	const float screenMargin = 10.f;
	const math::Vec sceneSize = APP->scene->box.size;
	const float menuTop = math::clamp(anchorPos.y, screenMargin,
	                                  std::max(screenMargin, sceneSize.y - screenMargin));
	const float availableHeight = std::max(1.f, sceneSize.y - menuTop - screenMargin);
	float totalHeight = 0.f;
	for (Widget* child : children)
		totalHeight += child->box.size.y;
	if (totalHeight <= availableHeight) {
		box.pos.y = menuTop;
		return;
	}

	int columnCount = 1;
	float columnHeight = 0.f;
	for (Widget* child : children) {
		const float itemHeight = child->box.size.y;
		if (columnHeight > 0.f && columnHeight + itemHeight > availableHeight) {
			columnCount++;
			columnHeight = 0.f;
		}
		columnHeight += itemHeight;
	}

	const float availableWidth = std::max(120.f, sceneSize.x - 2.f * screenMargin);
	const float naturalColumnWidth = std::max(150.f, box.size.x);
	const float columnWidth = std::min(naturalColumnWidth, availableWidth / columnCount);
	float x = 0.f;
	float y = 0.f;
	float tallestColumn = 0.f;
	for (Widget* child : children) {
		const float itemHeight = child->box.size.y;
		if (y > 0.f && y + itemHeight > availableHeight) {
			tallestColumn = std::max(tallestColumn, y);
			x += columnWidth;
			y = 0.f;
		}
		child->box.pos = math::Vec(x, y);
		child->box.size.x = columnWidth;
		y += itemHeight;
	}
	tallestColumn = std::max(tallestColumn, y);
	box.size = math::Vec(columnWidth * columnCount, tallestColumn);
	box.pos.x = math::clamp(anchorPos.x, screenMargin,
	                       std::max(screenMargin, sceneSize.x - box.size.x - screenMargin));
	box.pos.y = menuTop;
}

void DeepcacheSingleLineChoiceButton::draw(const DrawArgs& args) {
	const std::string fullText = text;
	const float availableWidth = std::max(1.f, box.size.x - 14.f);
	if (bndLabelWidth(args.vg, -1, text.c_str()) > availableWidth) {
		const std::string suffix = "...";
		text = suffix;
		for (std::size_t codepoints = fullText.size(); codepoints > 0; --codepoints) {
			const std::string prefix = string::truncate(fullText, codepoints);
			if (prefix == fullText)
				continue;
			const std::string candidate = string::trim(prefix) + suffix;
			if (bndLabelWidth(args.vg, -1, candidate.c_str()) <= availableWidth) {
				text = candidate;
				break;
			}
		}
	}
	ui::ChoiceButton::draw(args);
	text = fullText;
}

void DeepcacheBrandItem::onAction(const ActionEvent& e) {
	if (!browser || browserLifetime.expired())
		return;
	browser->brand = browser->brand == brand ? "" : brand;
	browser->refresh();
}

void DeepcacheBrandItem::step() {
	if (!browser || browserLifetime.expired()) {
		disabled = true;
		rightText.clear();
		ui::MenuItem::step();
		return;
	}
	rightText.clear();
	if (registeredModelCount > 0 && availableModelCount < registeredModelCount) {
		rightText = "A: " + std::to_string(availableModelCount) + "/" +
		            std::to_string(registeredModelCount);
	}
	if (browser->brand == brand) {
		if (!rightText.empty())
			rightText += "  ";
		rightText += CHECKMARK(true);
	}
	ui::MenuItem::step();
}

void DeepcacheBrandButton::onAction(const ActionEvent& e) {
	auto* menu = createMenu<DeepcacheMulticolumnMenu>();
	menu->anchorPos = getAbsoluteOffset(math::Vec(0, box.size.y));
	menu->box.pos = menu->anchorPos;
	menu->box.size.x = box.size.x;
	auto* allItem = new DeepcacheBrandItem;
	allItem->text = string::translate("Browser.allBrands");
	allItem->browser = browser;
	allItem->browserLifetime = browser->lifetimeToken;
	menu->addChild(allItem);
	menu->addChild(new ui::MenuSeparator);
	// The plugin binary can register more models than Rack's current library
	// manifest permits. Show that coverage gap rather than model->hidden, which
	// is only a browser presentation flag.
	std::map<std::string, std::pair<int, int>, string::CaseInsensitiveCompare> modelCountsByBrand;
	for (plugin::Plugin* plugin : plugin::plugins) {
		std::pair<int, int>& counts = modelCountsByBrand[plugin->brand];
		for (plugin::Model* model : plugin->models) {
			counts.second++;
			if (settings::isModuleWhitelisted(model->plugin->slug, model->slug))
				counts.first++;
		}
	}
	for (const auto& brandEntry : modelCountsByBrand) {
		auto* item = new DeepcacheBrandItem;
		item->text = brandEntry.first;
		item->brand = brandEntry.first;
		item->availableModelCount = brandEntry.second.first;
		item->registeredModelCount = brandEntry.second.second;
		item->browser = browser;
		item->browserLifetime = browser->lifetimeToken;
		menu->addChild(item);
	}
}

void DeepcacheBrandButton::step() {
	text = string::translate("Browser.brand");
	if (!browser->brand.empty())
		text += ": " + browser->brand;
	ui::ChoiceButton::step();
}

void DeepcacheTagItem::onAction(const ActionEvent& e) {
	if (!browser || browserLifetime.expired())
		return;
	if (tagId < 0) {
		browser->tagIds.clear();
		browser->refresh();
		return;
	}
	const auto selected = browser->tagIds.find(tagId);
	if (!e.isConsumed()) {
		if (selected == browser->tagIds.end())
			browser->tagIds.insert(tagId);
		else
			browser->tagIds.erase(selected);
		e.unconsume();
	}
	else {
		if (selected != browser->tagIds.end())
			browser->tagIds.clear();
		else
			browser->tagIds = {tagId};
	}
	browser->refresh();
}

void DeepcacheTagItem::step() {
	if (!browser || browserLifetime.expired()) {
		disabled = true;
		rightText.clear();
		ui::MenuItem::step();
		return;
	}
	rightText = CHECKMARK(tagId < 0 ? browser->tagIds.empty() : browser->tagIds.count(tagId) != 0);
	ui::MenuItem::step();
}

void DeepcacheTagButton::onAction(const ActionEvent& e) {
	auto* menu = createMenu<DeepcacheMulticolumnMenu>();
	menu->anchorPos = getAbsoluteOffset(math::Vec(0, box.size.y));
	menu->box.pos = menu->anchorPos;
	menu->box.size.x = box.size.x;
	auto* allItem = new DeepcacheTagItem;
	allItem->text = string::translate("Browser.allTags");
	allItem->tagId = -1;
	allItem->browser = browser;
	allItem->browserLifetime = browser->lifetimeToken;
	menu->addChild(allItem);
	menu->addChild(createMenuLabel(widget::getKeyCommandName(0, RACK_MOD_CTRL) +
	                                   string::translate("key.click") +
	                                   string::translate("Browser.tagsSelectMultiple")));
	menu->addChild(new ui::MenuSeparator);
	std::vector<int> sortedTagIds(tag::tagAliases.size());
	std::iota(sortedTagIds.begin(), sortedTagIds.end(), 0);
	std::sort(sortedTagIds.begin(), sortedTagIds.end(), [](int a, int b) {
		return string::translate("tag." + tag::getTag(a)) < string::translate("tag." + tag::getTag(b));
	});
	for (int tagId : sortedTagIds) {
		auto* item = new DeepcacheTagItem;
		item->text = string::translate("tag." + tag::getTag(tagId));
		item->tagId = tagId;
		item->browser = browser;
		item->browserLifetime = browser->lifetimeToken;
		menu->addChild(item);
	}
}

void DeepcacheTagButton::step() {
	text = string::translate("Browser.tags");
	if (!browser->tagIds.empty()) {
		text += ": ";
		bool first = true;
		for (int tagId : browser->tagIds) {
			if (!first)
				text += ", ";
			text += string::translate("tag." + tag::getTag(tagId));
			first = false;
		}
	}
	ui::ChoiceButton::step();
}

void DeepcacheSlugButton::onAction(const ActionEvent& e) {
	auto* menu = createMenu<DeepcacheMulticolumnMenu>();
	menu->anchorPos = getAbsoluteOffset(math::Vec(0, box.size.y));
	menu->box.pos = menu->anchorPos;
	menu->box.size.x = box.size.x;
	menu->addChild(createMenuLabel("Plugin slug → brand"));
	menu->addChild(new ui::MenuSeparator);

	std::vector<plugin::Plugin*> plugins = plugin::plugins;
	std::stable_sort(plugins.begin(), plugins.end(), [](const plugin::Plugin* a, const plugin::Plugin* b) {
		return lowercase(a->slug) < lowercase(b->slug);
	});
	for (const plugin::Plugin* plugin : plugins) {
		auto* item = new ui::MenuItem;
		item->text = plugin->slug;
		item->rightText = "→ " + plugin->brand;
		item->disabled = true;
		menu->addChild(item);
	}
}

void DeepcacheSlugButton::step() {
	text = "Slugs";
	ui::ChoiceButton::step();
}

void DeepcacheClearButton::onAction(const ActionEvent& e) {
	browser->clearFilters();
}

void DeepcacheSortButton::onAction(const ActionEvent& e) {
	ui::Menu* menu = createMenu();
	menu->box.pos = getAbsoluteOffset(math::Vec(0, box.size.y));
	menu->box.size.x = box.size.x;
	const auto names = deepcacheSortNames();
	DeepcacheBrowser* targetBrowser = browser;
	const std::weak_ptr<int> lifetime = browser ? browser->lifetimeToken : std::weak_ptr<int>();
	for (int sortId = settings::BROWSER_SORT_UPDATED; sortId <= settings::BROWSER_SORT_RANDOM; ++sortId) {
		menu->addChild(createCheckMenuItem(names[sortId], "",
			[sortId]() { return settings::browserSort == sortId; },
			[targetBrowser, lifetime, sortId]() {
				if (lifetime.expired())
					return;
				settings::browserSort = static_cast<settings::BrowserSort>(sortId);
				targetBrowser->refresh();
			}));
	}
}

void DeepcacheSortButton::step() {
	const auto names = deepcacheSortNames();
	const int sortId = math::clamp(static_cast<int>(settings::browserSort),
	                               static_cast<int>(settings::BROWSER_SORT_UPDATED),
	                               static_cast<int>(settings::BROWSER_SORT_RANDOM));
	text = string::translate("Browser.sort") + names[sortId];
	text = string::ellipsize(text, 20);
	ui::ChoiceButton::step();
}

void DeepcacheZoomButton::onAction(const ActionEvent& e) {
	ui::Menu* menu = createMenu();
	menu->box.pos = getAbsoluteOffset(math::Vec(0, box.size.y));
	menu->box.size.x = box.size.x;
	for (float zoom = 1.f; zoom >= -2.f; zoom -= 0.5f) {
		menu->addChild(createCheckMenuItem(string::f("%.0f%%", std::pow(2.f, zoom) * 100.f), "",
			[zoom]() { return settings::browserZoom == zoom; },
			[zoom]() { settings::browserZoom = zoom; }));
	}
}

void DeepcacheZoomButton::step() {
	text = string::f("Zoom: %.0f%%", std::pow(2.f, settings::browserZoom) * 100.f);
	ui::ChoiceButton::step();
}

struct DeepcacheProgressWidget : widget::TransparentWidget {
	enum class Stage {
		MODULES,
		FRAMEBUFFERS
	};

	DeepcacheModule* module = nullptr;
	Stage stage = Stage::MODULES;
	DeepcacheProgressWidget(DeepcacheModule* module, Stage stage)
		: module(module), stage(stage) {
	}
	void draw(const DrawArgs& args) override {
		const bool framebufferStage = stage == Stage::FRAMEBUFFERS;
		const int completed = !module ? 0 : framebufferStage
			? module->framebufferPluginCompletedCount.load(std::memory_order_relaxed)
			: module->constructionPluginCompletedCount.load(std::memory_order_relaxed);
		const int total = !module ? 0 : framebufferStage
			? module->framebufferPluginTotalCount.load(std::memory_order_relaxed)
			: module->constructionPluginTotalCount.load(std::memory_order_relaxed);
		const auto cacheState = module ? static_cast<deepcache::CacheState>(
			module->cacheState.load(std::memory_order_relaxed)) : deepcache::CacheState::IDLE;
		const float progress = total > 0 ? math::clamp(completed / static_cast<float>(total), 0.f, 1.f)
		                               : (!framebufferStage && cacheState == deepcache::CacheState::READY ? 1.f : 0.f);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.75f, 0.75f, box.size.x - 1.5f, box.size.y - 1.5f, 2.f);
		nvgFillColor(args.vg, nvgRGBA(8, 13, 22, 235));
		nvgFill(args.vg);
		if (progress > 0.f) {
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, 2.f, 2.f, (box.size.x - 4.f) * progress, box.size.y - 4.f, 1.25f);
			nvgFillColor(args.vg, framebufferStage ? nvgRGBA(35, 205, 225, 220)
			                                               : nvgRGBA(139, 104, 255, 220));
			nvgFill(args.vg);
		}
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.75f, 0.75f, box.size.x - 1.5f, box.size.y - 1.5f, 2.f);
		nvgStrokeWidth(args.vg, 1.5f);
		nvgStrokeColor(args.vg, framebufferStage ? nvgRGBA(56, 221, 232, 190)
		                                               : nvgRGBA(139, 104, 255, 190));
		nvgStroke(args.vg);
		nvgFontSize(args.vg, 8.f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		const std::string text = std::to_string(static_cast<int>(std::round(progress * 100.f))) + "%";
		const float textX = box.size.x * 0.5f;
		const float textY = box.size.y * 0.52f;
		// A small dark offset keeps the white percentage legible over both the
		// violet and cyan fills without making the compact type look outlined.
		nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 156));
		nvgText(args.vg, textX + 0.50f, textY + 0.56f, text.c_str(), nullptr);
		nvgFillColor(args.vg, color::WHITE);
		nvgText(args.vg, textX, textY, text.c_str(), nullptr);
	}
};

struct DeepcacheModuleCountWidget : widget::TransparentWidget {
	DeepcacheModule* module = nullptr;
	explicit DeepcacheModuleCountWidget(DeepcacheModule* module)
		: module(module) {
	}
	void draw(const DrawArgs& args) override {
		const int completed = module ? module->constructionPluginCompletedCount.load(std::memory_order_relaxed) : 0;
		const std::string text = std::to_string(completed);
		nvgFontSize(args.vg, 8.f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, color::WHITE);
		nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.5f, text.c_str(), nullptr);
	}
};

struct DeepcacheDatabaseStatusWidget : widget::TransparentWidget {
	DeepcacheModule* module = nullptr;
	explicit DeepcacheDatabaseStatusWidget(DeepcacheModule* module) : module(module) {}

	void draw(const DrawArgs& args) override {
		const bool browserStandby = module && module->browserStandby.load(std::memory_order_relaxed);
		const bool duplicateInstance = module && module->duplicateInstance.load(std::memory_order_relaxed);
		const bool ownershipConflict = module && module->browserOwnershipConflict.load(std::memory_order_relaxed);
		const auto rawState = module ? static_cast<deepcache::DatabaseState>(
			module->databaseState.load(std::memory_order_relaxed)) : deepcache::DatabaseState::EMPTY;
		const auto cacheState = module ? static_cast<deepcache::CacheState>(
			module->cacheState.load(std::memory_order_relaxed)) : deepcache::CacheState::IDLE;
		const int ready = module ? module->databaseReadyPluginCount.load(std::memory_order_relaxed) : 0;
		const int target = module ? module->databaseTargetPluginCount.load(std::memory_order_relaxed) : 0;
		const bool cachePassActive = cacheState == deepcache::CacheState::PLANNING ||
		                             cacheState == deepcache::CacheState::WARMING;
		// The archive worker can briefly drain its queue between UI-thread
		// framebuffer submissions. Keep the user-facing state stable across those
		// gaps, while preserving higher-priority lifecycle and error states.
		deepcache::DatabaseState state = rawState;
		if (cachePassActive && target > 0 && ready < target &&
		    (rawState == deepcache::DatabaseState::EMPTY ||
		     rawState == deepcache::DatabaseState::READY ||
		     rawState == deepcache::DatabaseState::UPDATING))
			state = deepcache::DatabaseState::UPDATING;
		const char* status = ownershipConflict ? "CONFLICT" :
		                     duplicateInstance ? "DUPLICATE" :
		                     browserStandby ? "STANDBY" : "EMPTY";
		if (!browserStandby && !duplicateInstance && !ownershipConflict) {
			switch (state) {
				case deepcache::DatabaseState::LOADING: status = "LOADING"; break;
				case deepcache::DatabaseState::READY: status = "READY"; break;
				case deepcache::DatabaseState::UPDATING: status = "UPDATING"; break;
				case deepcache::DatabaseState::COMPACTING: status = "COMPACTING"; break;
				case deepcache::DatabaseState::CANCELING: status = "CANCELING"; break;
				case deepcache::DatabaseState::BUSY: status = "MEMORY ONLY"; break;
				case deepcache::DatabaseState::READ_ONLY: status = "READ ONLY"; break;
				case deepcache::DatabaseState::ERROR: status = "ERROR"; break;
				case deepcache::DatabaseState::EMPTY: break;
			}
		}
		const std::uint64_t bytes = module ? module->databaseBytes.load(std::memory_order_relaxed) : 0;
		std::string pluginCount = "Plugins: " + std::to_string(ready);
		if (!ownershipConflict && !duplicateInstance && !browserStandby && target > 0 && ready < target &&
		    (state == deepcache::DatabaseState::LOADING || state == deepcache::DatabaseState::UPDATING))
			pluginCount += "/" + std::to_string(target);
		const std::string databaseSize = string::f("%.1f MB", bytes / (1024.0 * 1024.0));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFontSize(args.vg, 8.f);
		nvgFillColor(args.vg, color::WHITE);
		nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.17f, status, nullptr);
		nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.50f, pluginCount.c_str(), nullptr);
		nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.83f, databaseSize.c_str(), nullptr);
	}
};

// Rack Pro can host several independent Rack contexts in one DAW process, while
// this plugin DLL's globals are shared by all of them. Scene::browser is the
// actual collision domain, so permit one Deepcache owner per Scene.
std::mutex gDeepcacheSceneOwnersMutex;
std::unordered_map<app::Scene*, DeepcacheWidget*> gDeepcacheSceneOwners;

bool claimDeepcacheScene(app::Scene* scene, DeepcacheWidget* widget) {
	if (!scene || !widget)
		return false;
	std::lock_guard<std::mutex> lock(gDeepcacheSceneOwnersMutex);
	const auto existing = gDeepcacheSceneOwners.find(scene);
	if (existing != gDeepcacheSceneOwners.end())
		return existing->second == widget;
	gDeepcacheSceneOwners.emplace(scene, widget);
	return true;
}

void releaseDeepcacheScene(app::Scene* scene, DeepcacheWidget* widget) {
	if (!scene || !widget)
		return;
	std::lock_guard<std::mutex> lock(gDeepcacheSceneOwnersMutex);
	const auto existing = gDeepcacheSceneOwners.find(scene);
	if (existing != gDeepcacheSceneOwners.end() && existing->second == widget)
		gDeepcacheSceneOwners.erase(existing);
}

bool isStoermelderMb(const plugin::Model* model) {
	if (!model || !model->plugin || model->slug != "Mb")
		return false;
	return model->plugin->slug == "Stoermelder-P1" ||
	       model->plugin->slug == "Stoermelder-PackTau";
}

bool rackContainsStoermelderMb(app::Scene* scene) {
	if (!scene || !scene->rack)
		return false;
	for (app::ModuleWidget* moduleWidget : scene->rack->getModules()) {
		if (moduleWidget && isStoermelderMb(moduleWidget->model))
			return true;
	}
	return false;
}

}  // namespace

DeepcacheModule::DeepcacheModule() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
}

void DeepcacheModule::process(const ProcessArgs& args) {
	(void)args;
	const auto state = static_cast<deepcache::CacheState>(cacheState.load(std::memory_order_relaxed));
	const bool duplicate = duplicateInstance.load(std::memory_order_relaxed);
	const bool ownershipConflict = browserOwnershipConflict.load(std::memory_order_relaxed);
	const bool databaseError = static_cast<deepcache::DatabaseState>(
		databaseState.load(std::memory_order_relaxed)) == deepcache::DatabaseState::ERROR;
	lights[PLANNING_LIGHT].setBrightness(state == deepcache::CacheState::PLANNING ? 1.f : 0.f);
	lights[WARMING_LIGHT].setBrightness(state == deepcache::CacheState::WARMING ? 1.f :
	                                    state == deepcache::CacheState::PAUSED ? 0.3f : 0.f);
	lights[READY_LIGHT].setBrightness(state == deepcache::CacheState::READY ? 1.f : 0.f);
	lights[ERROR_LIGHT].setBrightness(
		state == deepcache::CacheState::ERROR || databaseError || duplicate || ownershipConflict ? 1.f : 0.f);
}

json_t* DeepcacheModule::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "uiBudgetMs", json_real(uiBudgetMicros.load(std::memory_order_relaxed) / 1000.0));
	return root;
}

void DeepcacheModule::dataFromJson(json_t* root) {
	if (json_t* value = json_object_get(root, "uiBudgetMs")) {
		const double budget = std::max(0.5, std::min(8.0, json_number_value(value)));
		uiBudgetMicros.store(static_cast<int>(std::round(budget * 1000.0)), std::memory_order_relaxed);
	}
}

struct DeepcacheWidget::Internal {
	DeepcacheModule* module = nullptr;
	app::Scene* scene = nullptr;
	PreviewCacheManager* cacheManager = nullptr;
	DeepcacheBrowserOverlay* overlay = nullptr;
	DeepcacheWarmRenderHost* warmRenderHost = nullptr;
	bool active = false;
	bool startPending = false;
	bool ownershipConflictHandled = false;
	double nextActivationCheck = 0.0;
};

void DeepcacheWidget::activate() {
	if (!internal_ || internal_->active || !internal_->module || !internal_->scene)
		return;
	internal_->active = true;
	internal_->module->browserStandby.store(false, std::memory_order_relaxed);
	internal_->module->duplicateInstance.store(false, std::memory_order_relaxed);
	internal_->module->browserOwnershipConflict.store(false, std::memory_order_relaxed);
	internal_->cacheManager = new PreviewCacheManager(internal_->module);
	internal_->overlay = new DeepcacheBrowserOverlay(internal_->cacheManager, internal_->scene);
	if (internal_->overlay->installed && internal_->overlay->browser) {
		internal_->cacheManager->setBrowser(internal_->overlay->browser);
		internal_->warmRenderHost = new DeepcacheWarmRenderHost;
		internal_->warmRenderHost->cacheManager = internal_->cacheManager;
		internal_->scene->addChild(internal_->warmRenderHost);
		internal_->startPending = true;
	}
	else {
		internal_->module->cacheState.store(static_cast<int>(deepcache::CacheState::ERROR), std::memory_order_relaxed);
	}
}

DeepcacheWidget::DeepcacheWidget(DeepcacheModule* module) {
	setModule(module);
	visual_assets::SplitPanelRenderer splitPanel(this, "res/Deepcache.panel.svg");
	const std::string& panelPath = splitPanel.panelPath();
	splitPanel.addLabels("res/Deepcache.labels.svg");
	splitPanel.addPerfectWaveSoloBranding();

	math::Vec planningMm(4.5f, 91.59958f);
	math::Vec warmingMm(4.5f, 98.59958f);
	math::Vec readyMm(4.5f, 105.59958f);
	math::Vec errorMm(4.5f, 112.59958f);
	panel_svg::loadPointFromSvgMm(panelPath, "planning_light", &planningMm);
	panel_svg::loadPointFromSvgMm(panelPath, "warming_light", &warmingMm);
	panel_svg::loadPointFromSvgMm(panelPath, "ready_light", &readyMm);
	panel_svg::loadPointFromSvgMm(panelPath, "error_light", &errorMm);

	math::Rect progressRectMm(math::Vec(3.f, 27.099751f), math::Vec(14.32f, 7.f));
	math::Rect moduleCountRectMm(math::Vec(3.f, 34.299751f), math::Vec(14.32f, 4.5f));
	math::Rect framebufferProgressRectMm(math::Vec(3.f, 49.599771f), math::Vec(14.32f, 7.f));
	math::Rect databaseStatusRectMm(math::Vec(3.f, 70.f), math::Vec(14.32f, 12.186809f));
	panel_svg::loadRectFromSvgMm(panelPath, "progress", &progressRectMm);
	panel_svg::loadRectFromSvgMm(panelPath, "progress_count", &moduleCountRectMm);
	panel_svg::loadRectFromSvgMm(panelPath, "framebuffer_progress", &framebufferProgressRectMm);
	panel_svg::loadRectFromSvgMm(panelPath, "database_status", &databaseStatusRectMm);
	auto* progress = new DeepcacheProgressWidget(module, DeepcacheProgressWidget::Stage::MODULES);
	progress->box.pos = mm2px(progressRectMm.pos);
	progress->box.size = mm2px(progressRectMm.size);
	addChild(progress);
	auto* moduleCount = new DeepcacheModuleCountWidget(module);
	moduleCount->box.pos = mm2px(moduleCountRectMm.pos);
	moduleCount->box.size = mm2px(moduleCountRectMm.size);
	addChild(moduleCount);
	auto* framebufferProgress = new DeepcacheProgressWidget(module, DeepcacheProgressWidget::Stage::FRAMEBUFFERS);
	framebufferProgress->box.pos = mm2px(framebufferProgressRectMm.pos);
	framebufferProgress->box.size = mm2px(framebufferProgressRectMm.size);
	addChild(framebufferProgress);
	auto* databaseStatus = new DeepcacheDatabaseStatusWidget(module);
	databaseStatus->box.pos = mm2px(databaseStatusRectMm.pos);
	databaseStatus->box.size = mm2px(databaseStatusRectMm.size);
	addChild(databaseStatus);
	addChild(createLightCentered<SmallAperture<AmberApertureLight>>(mm2px(planningMm), module, DeepcacheModule::PLANNING_LIGHT));
	addChild(createLightCentered<SmallAperture<BlueApertureLight>>(mm2px(warmingMm), module, DeepcacheModule::WARMING_LIGHT));
	addChild(createLightCentered<SmallAperture<GreenApertureLight>>(mm2px(readyMm), module, DeepcacheModule::READY_LIGHT));
	addChild(createLightCentered<SmallAperture<RedApertureLight>>(mm2px(errorMm), module, DeepcacheModule::ERROR_LIGHT));

	// Browser previews must remain completely inert.
	if (!module)
		return;
	internal_ = new Internal;
	internal_->module = module;
	internal_->scene = APP ? APP->scene : nullptr;
	module->browserStandby.store(false, std::memory_order_relaxed);
	module->duplicateInstance.store(false, std::memory_order_relaxed);
	module->browserOwnershipConflict.store(false, std::memory_order_relaxed);
	if (!internal_->scene) {
		module->cacheState.store(static_cast<int>(deepcache::CacheState::ERROR), std::memory_order_relaxed);
		return;
	}
	// MB owns and mutates the same raw Scene::browser slot. In particular, its
	// live browser is not safe to detach/traverse as though it were Rack's stock
	// browser. Decline installation before touching that pointer. The module can
	// remain inert in the patch without affecting MB. Report the ownership
	// collision consistently as a conflict regardless of module spawn order;
	// browserStandby still records that this safe case can activate automatically
	// after MB is removed.
	if (rackContainsStoermelderMb(internal_->scene)) {
		module->browserStandby.store(true, std::memory_order_relaxed);
		module->browserOwnershipConflict.store(true, std::memory_order_relaxed);
		module->cacheState.store(static_cast<int>(deepcache::CacheState::DISABLED), std::memory_order_relaxed);
		return;
	}
	if (claimDeepcacheScene(internal_->scene, this)) {
		activate();
	}
	else {
		module->duplicateInstance.store(true, std::memory_order_relaxed);
		module->cacheState.store(static_cast<int>(deepcache::CacheState::DISABLED), std::memory_order_relaxed);
	}
}

DeepcacheWidget::~DeepcacheWidget() {
	if (!internal_)
		return;
	if (internal_->active) {
		if (internal_->warmRenderHost) {
			internal_->warmRenderHost->cacheManager = nullptr;
			if (internal_->warmRenderHost->parent)
				internal_->warmRenderHost->parent->removeChild(internal_->warmRenderHost);
			delete internal_->warmRenderHost;
			internal_->warmRenderHost = nullptr;
		}
		if (internal_->cacheManager)
			internal_->cacheManager->stop();
		if (internal_->overlay) {
			if (internal_->overlay->ownsBrowserSlot()) {
				if (internal_->overlay->browser)
					releaseFramebuffers(internal_->overlay->browser);
				internal_->overlay->restore();
				delete internal_->overlay;
			}
			else if (internal_->overlay->installed) {
				// A successor browser may hold this raw pointer as its backup. Keep a
				// retired chain link instead of handing it a dangling pointer.
				internal_->overlay->retireForSuccessor();
			}
			else {
				delete internal_->overlay;
			}
		}
		delete internal_->cacheManager;
		releaseDeepcacheScene(internal_->scene, this);
	}
	delete internal_;
	internal_ = nullptr;
}

void DeepcacheWidget::step() {
	if (internal_ && !internal_->active && internal_->module && internal_->scene) {
		const double now = system::getTime();
		if (now >= internal_->nextActivationCheck) {
			internal_->nextActivationCheck = now + 1.0;
			if (rackContainsStoermelderMb(internal_->scene)) {
				internal_->module->browserStandby.store(true, std::memory_order_relaxed);
				internal_->module->duplicateInstance.store(false, std::memory_order_relaxed);
				internal_->module->browserOwnershipConflict.store(true, std::memory_order_relaxed);
				internal_->module->cacheState.store(static_cast<int>(deepcache::CacheState::DISABLED), std::memory_order_relaxed);
			}
			else if (claimDeepcacheScene(internal_->scene, this)) {
				activate();
			}
			else {
				internal_->module->browserStandby.store(false, std::memory_order_relaxed);
				internal_->module->duplicateInstance.store(true, std::memory_order_relaxed);
				internal_->module->browserOwnershipConflict.store(false, std::memory_order_relaxed);
			}
		}
	}
	if (internal_ && internal_->active && internal_->cacheManager) {
		// Rack exposes no browser-owner notification. Once installed, pointer
		// identity is enough to detect a successor without inspecting or mutating it.
		if (!internal_->ownershipConflictHandled && internal_->overlay && internal_->overlay->installed &&
		    !internal_->overlay->ownsBrowserSlot()) {
			internal_->ownershipConflictHandled = true;
			internal_->startPending = false;
			internal_->overlay->ownershipConflict = true;
			if (internal_->overlay->dragon) {
				internal_->overlay->dragon->enabled = false;
				internal_->overlay->dragon->hide();
			}
			internal_->module->browserOwnershipConflict.store(true, std::memory_order_relaxed);
			if (internal_->warmRenderHost)
				internal_->warmRenderHost->cacheManager = nullptr;
			internal_->cacheManager->stop();
		}
		if (!internal_->ownershipConflictHandled) {
			if (internal_->startPending) {
				internal_->startPending = false;
				internal_->cacheManager->start();
			}
			internal_->cacheManager->step();
		}
	}
	ModuleWidget::step();
}

void DeepcacheWidget::appendContextMenu(ui::Menu* menu) {
	ModuleWidget::appendContextMenu(menu);
	if (!internal_ || !internal_->module)
		return;
	menu->addChild(new ui::MenuSeparator);
	if (internal_->module->browserStandby.load(std::memory_order_relaxed)) {
		menu->addChild(createMenuLabel("Conflict: Stoermelder MB owns the browser"));
		menu->addChild(createMenuLabel("Deepcache activates automatically when MB is removed"));
		return;
	}
	if (!internal_->active) {
		menu->addChild(createMenuLabel("Duplicate: another Deepcache owns the browser"));
		menu->addChild(createMenuLabel("This copy is inactive and can be removed"));
		return;
	}
	if (internal_->module->browserOwnershipConflict.load(std::memory_order_relaxed)) {
		menu->addChild(createMenuLabel("Conflict: another plugin replaced the browser"));
		menu->addChild(createMenuLabel("Deepcache background work has stopped"));
		return;
	}
	PreviewCacheManager* manager = internal_->cacheManager;
	const std::weak_ptr<int> lifetime = manager->lifetimeToken();
	menu->addChild(createMenuItem("Rebuild cache", "", [manager, lifetime]() { if (!lifetime.expired()) manager->rebuild(); }));
	menu->addChild(createSubmenuItem("Cache resolution", "", [manager, lifetime](ui::Menu* child) {
		if (lifetime.expired()) {
			child->addChild(createMenuLabel("Deepcache is no longer available"));
			return;
		}
		for (int percent : {100, 200}) {
			child->addChild(createCheckMenuItem(std::to_string(percent) + "%", "",
				[manager, lifetime, percent]() {
					return !lifetime.expired() && manager->previewCacheResolutionPercent() == percent;
				},
				[manager, lifetime, percent]() {
					if (!lifetime.expired())
						manager->setPreviewCacheResolutionPercent(percent);
				}));
		}
	}));
	menu->addChild(new ui::MenuSeparator);
	DeepcacheModule* deepcacheModule = internal_->module;
	menu->addChild(createSubmenuItem("UI work budget", "", [deepcacheModule, lifetime](ui::Menu* child) {
		if (lifetime.expired()) {
			child->addChild(createMenuLabel("Deepcache is no longer available"));
			return;
		}
		for (int micros : {500, 1000, 2000, 4000, 8000}) {
			const std::string label = micros < 1000 ? "0.5 ms" : std::to_string(micros / 1000) + " ms";
			child->addChild(createCheckMenuItem(label, "",
				[deepcacheModule, lifetime, micros]() { return !lifetime.expired() && deepcacheModule->uiBudgetMicros.load(std::memory_order_relaxed) == micros; },
				[deepcacheModule, lifetime, micros]() { if (!lifetime.expired()) deepcacheModule->uiBudgetMicros.store(micros, std::memory_order_relaxed); }));
		}
	}));
	menu->addChild(createSubmenuItem("Cache statistics", "", [manager, lifetime](ui::Menu* child) {
		if (lifetime.expired()) {
			child->addChild(createMenuLabel("Deepcache is no longer available"));
			return;
		}
		child->addChild(createMenuLabel("Completed: " + std::to_string(manager->completedCount()) +
		                                    " / " + std::to_string(manager->totalCount())));
		child->addChild(createMenuLabel("Resident records: " + std::to_string(manager->residentRecordCount())));
		child->addChild(createMenuLabel("Resident previews: " + std::to_string(manager->residentPreviewCount())));
		child->addChild(createMenuLabel("Framebuffer-ready: " + std::to_string(manager->framebufferReadyPreviewCount())));
		child->addChild(createMenuLabel(string::f("Volatile QOI: %.1f MB", manager->hotQoiBytes() / (1024.0 * 1024.0))));
		child->addChild(createMenuLabel(string::f("Retained RGBA: %.1f MB", manager->retainedRgbaBytes() / (1024.0 * 1024.0))));
		child->addChild(createMenuLabel(string::f("Pending upload RGBA: %.1f MB", manager->pendingUploadBytes() / (1024.0 * 1024.0))));
		child->addChild(createMenuLabel(string::f("Estimated GPU RGBA: %.1f MB", manager->estimatedGpuBytes() / (1024.0 * 1024.0))));
		child->addChild(createMenuLabel("Failed: " + std::to_string(manager->failedCount())));
		child->addChild(createMenuLabel(string::f("Average construction: %.2f ms", manager->averageConstructionMs())));
		child->addChild(createMenuLabel(string::f("Maximum construction: %.2f ms", manager->maximumConstructionMs())));
	}));
	if (internal_->overlay && internal_->overlay->ownershipConflict)
		menu->addChild(createMenuLabel("Warning: another plugin currently owns the browser"));
}

Model* modelDeepcache = createModel<DeepcacheModule, DeepcacheWidget>("Deepcache");
