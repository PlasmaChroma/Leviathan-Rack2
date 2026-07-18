#include "Deepcache.hpp"

#include "DeepcacheBrowserLogic.hpp"
#include "PanelSvgUtils.hpp"

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
#include <deque>
#include <exception>
#include <memory>
#include <map>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
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
	RETRY,
	FAILED
};

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
	ui::Tooltip* tooltip = nullptr;
	deepcache::PreviewEntryState state = deepcache::PreviewEntryState::EMPTY;
	std::string failureReason;

	DeepcacheModelBox(plugin::Model* model, std::size_t modelIndex, int pluginModelOrder,
	                  PreviewCacheManager* manager);
	~DeepcacheModelBox() override;
	bool ensurePreviewConstructed();
	FramebufferWarmResult warmFramebuffer();
	bool hasValidFramebufferImage() const;
	void invalidateFramebufferReadiness();
	void clearPreview();
	void updateZoom();
	void draw(const DrawArgs& args) override;
	void onButton(const ButtonEvent& e) override;
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

struct DeepcacheBrandMenu : ui::Menu {
	math::Vec anchorPos;
	void step() override;
};

struct DeepcacheBrandItem : ui::MenuItem {
	DeepcacheBrowser* browser = nullptr;
	std::string brand;
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheBrandButton : ui::ChoiceButton {
	DeepcacheBrowser* browser = nullptr;
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheTagItem : ui::MenuItem {
	DeepcacheBrowser* browser = nullptr;
	int tagId = -1;
	void onAction(const ActionEvent& e) override;
	void step() override;
};

struct DeepcacheTagButton : ui::ChoiceButton {
	DeepcacheBrowser* browser = nullptr;
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

	explicit DeepcacheBrowser(PreviewCacheManager* manager);
	~DeepcacheBrowser() override;
	void step() override;
	void draw(const DrawArgs& args) override;
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
	std::size_t residentPreviewCount() const;
	std::size_t framebufferReadyPreviewCount() const;
	void invalidateFramebufferReadiness();
	app::ModuleWidget* chooseModel(plugin::Model* model);
};

class PreviewCacheManager {
public:
	explicit PreviewCacheManager(DeepcacheModule* module)
		: module_(module) {
		publish();
	}

	~PreviewCacheManager() {
		stop();
	}

	void setBrowser(DeepcacheBrowser* browser) {
		browser_ = browser;
	}

	deepcache::CacheState state() const {
		return state_;
	}

	std::uint64_t generation() const {
		return activeGeneration_;
	}

	void start() {
		if (!browser_ || state_ == deepcache::CacheState::STOPPING)
			return;
		if (state_ == deepcache::CacheState::PLANNING || state_ == deepcache::CacheState::WARMING ||
		    state_ == deepcache::CacheState::PAUSED)
			cancel();

		activeGeneration_ = ++nextGeneration_;
		completed_ = 0;
		total_ = 0;
		failed_ = 0;
		constructionTarget_ = 0;
		constructionCompleted_ = 0;
		constructionFailed_ = 0;
		framebufferTarget_ = 0;
		framebufferCompleted_ = 0;
		framebufferWarmQueue_.clear();
		generationResidentIndices_.clear();
		framebufferWarmForGeneration_ = module_->experimentalFramebufferWarm.load(std::memory_order_relaxed);
		constructionTotalMs_ = 0.0;
		constructionMaxMs_ = 0.0;
		constructedCount_ = 0;
		warmingStartedAt_ = system::getTime();
		deepcache::PreviewPlanInput input;
		input.generation = activeGeneration_;
		input.scope = static_cast<deepcache::CacheScope>(module_->cacheScope.load(std::memory_order_relaxed));
		input.visibleModelIndices = browser_->visibleModelIndices();
		worker_.resume();
		worker_.submit(browser_->snapshotModelDescriptors(), std::move(input));
		setState(deepcache::CacheState::PLANNING);
	}

	void rebuild() {
		clear();
		start();
	}

	void pause() {
		if (state_ != deepcache::CacheState::WARMING)
			return;
		worker_.pause();
		setState(deepcache::CacheState::PAUSED);
	}

	void resume() {
		if (state_ != deepcache::CacheState::PAUSED)
			return;
		worker_.resume();
		setState(deepcache::CacheState::WARMING);
	}

	void cancel() {
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
		framebufferWarmQueue_.clear();
		generationResidentIndices_.clear();
		setState(deepcache::CacheState::IDLE);
	}

	void clear() {
		if (!browser_)
			return;
		if (activeGeneration_ != 0)
			worker_.cancel(activeGeneration_);
		activeGeneration_ = 0;
		setState(deepcache::CacheState::CLEARING);
		browser_->clearPreviews();
		backend_.clear();
		completed_ = 0;
		total_ = 0;
		failed_ = 0;
		constructionTarget_ = 0;
		constructionCompleted_ = 0;
		constructionFailed_ = 0;
		framebufferTarget_ = 0;
		framebufferCompleted_ = 0;
		framebufferWarmQueue_.clear();
		generationResidentIndices_.clear();
		setState(deepcache::CacheState::IDLE);
	}

	void stop() {
		if (stopped_)
			return;
		stopped_ = true;
		state_ = deepcache::CacheState::STOPPING;
		publish();
		worker_.shutdown();
		activeGeneration_ = 0;
	}

	void promote(std::size_t modelIndex) {
		if (activeGeneration_ != 0)
			worker_.promote(modelIndex, activeGeneration_);
	}

	void handleButtonPress() {
		switch (state_) {
			case deepcache::CacheState::IDLE:
			case deepcache::CacheState::DISABLED:
				start();
				break;
			case deepcache::CacheState::PLANNING:
				cancel();
				break;
			case deepcache::CacheState::WARMING:
				pause();
				break;
			case deepcache::CacheState::PAUSED:
				resume();
				break;
			case deepcache::CacheState::READY:
				rebuild();
				break;
			case deepcache::CacheState::ERROR:
				start();
				break;
			case deepcache::CacheState::CLEARING:
			case deepcache::CacheState::STOPPING:
				break;
		}
	}

	void setFramebufferWarmEnabled(bool enabled) {
		module_->experimentalFramebufferWarm.store(enabled, std::memory_order_relaxed);
		framebufferWarmForGeneration_ = enabled;
		framebufferWarmQueue_.clear();
		if (constructionTarget_ <= 0) {
			publish();
			return;
		}

		if (!enabled) {
			failed_ = constructionFailed_;
			completed_ = constructionCompleted_;
			framebufferTarget_ = 0;
			framebufferCompleted_ = 0;
			if (constructionCompleted_ >= constructionTarget_ && state_ != deepcache::CacheState::PAUSED)
				setState(deepcache::CacheState::READY);
			else
				publish();
			return;
		}

		failed_ = constructionFailed_;
		completed_ = constructionFailed_;
		framebufferTarget_ = static_cast<int>(generationResidentIndices_.size());
		framebufferCompleted_ = 0;
		for (std::size_t modelIndex : generationResidentIndices_) {
			DeepcacheModelBox* box = browser_ ? browser_->getModelBox(modelIndex) : nullptr;
			if (box && box->state == deepcache::PreviewEntryState::FRAMEBUFFER_READY) {
				completed_++;
				framebufferCompleted_++;
			}
			else
				framebufferWarmQueue_.push_back({modelIndex, 0});
		}
		if (state_ == deepcache::CacheState::READY && completed_ < total_)
			setState(deepcache::CacheState::WARMING);
		else {
			publish();
			finishIfComplete();
		}
	}

	void onGraphicsContextDestroy() {
		if (!browser_)
			return;
		browser_->invalidateFramebufferReadiness();
		if (!framebufferWarmForGeneration_ || constructionTarget_ <= 0)
			return;
		framebufferWarmQueue_.clear();
		completed_ = constructionFailed_;
		failed_ = constructionFailed_;
		framebufferTarget_ = static_cast<int>(generationResidentIndices_.size());
		framebufferCompleted_ = 0;
		for (std::size_t modelIndex : generationResidentIndices_)
			framebufferWarmQueue_.push_back({modelIndex, 0});
		if (state_ == deepcache::CacheState::READY)
			setState(deepcache::CacheState::WARMING);
		else
			publish();
	}

	void step() {
		if (stopped_ || !browser_)
			return;

		if (state_ == deepcache::CacheState::PLANNING && worker_.isPlanReady(activeGeneration_)) {
			total_ = static_cast<int>(worker_.plannedRequestCount(activeGeneration_));
			constructionTarget_ = total_;
			if (total_ == 0)
				setState(deepcache::CacheState::READY);
			else
				setState(deepcache::CacheState::WARMING);
		}

		if (state_ != deepcache::CacheState::WARMING)
			return;

		const double frameStart = system::getTime();
		const double budgetMs = std::max(0.5, module_->uiBudgetMicros.load(std::memory_order_relaxed) / 1000.0);
		int processedThisFrame = 0;
		while (processedThisFrame < 4) {
			if (processedThisFrame > 0 && (system::getTime() - frameStart) * 1000.0 >= budgetMs)
				break;
			deepcache::PreviewBuildRequest request;
			if (!worker_.tryPop(request))
				break;
			if (request.generation != activeGeneration_)
				continue;
			DeepcacheModelBox* box = browser_->getModelBox(request.modelIndex);
			if (!box) {
				constructionCompleted_++;
				constructionFailed_++;
				completed_++;
				failed_++;
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
				failed_++;
				constructionFailed_++;
				backend_.invalidate(request.cacheKey);
			}
			else {
				generationResidentIndices_.insert(request.modelIndex);
				backend_.store(request.cacheKey);
				if (framebufferWarmForGeneration_) {
					if (box->state == deepcache::PreviewEntryState::FRAMEBUFFER_READY) {
						completed_++;
						framebufferCompleted_++;
					}
					else
						framebufferWarmQueue_.push_back({request.modelIndex, 0});
				}
			}
			constructionCompleted_++;
			if (!framebufferWarmForGeneration_ || !resident)
				completed_++;
			processedThisFrame++;
			publish();
		}
		if (framebufferWarmForGeneration_ && constructionCompleted_ >= constructionTarget_) {
			framebufferTarget_ = static_cast<int>(generationResidentIndices_.size());
			publish();
		}
		finishIfComplete();
	}

	void warmFramebuffers() {
		if (stopped_ || !browser_ || state_ != deepcache::CacheState::WARMING ||
		    !framebufferWarmForGeneration_ || constructionCompleted_ < constructionTarget_)
			return;
		const double frameStart = system::getTime();
		const double budgetMs = std::max(0.5, module_->uiBudgetMicros.load(std::memory_order_relaxed) / 1000.0);
		int processedThisFrame = 0;
		while (processedThisFrame < 4 && !framebufferWarmQueue_.empty()) {
			if (processedThisFrame > 0 && (system::getTime() - frameStart) * 1000.0 >= budgetMs)
				break;
			auto request = framebufferWarmQueue_.front();
			framebufferWarmQueue_.pop_front();
			DeepcacheModelBox* box = browser_->getModelBox(request.first);
			const double startedAt = system::getTime();
			const FramebufferWarmResult result = box ? box->warmFramebuffer() : FramebufferWarmResult::FAILED;
			const double durationMs = (system::getTime() - startedAt) * 1000.0;
			processedThisFrame++;
			if (result == FramebufferWarmResult::RETRY && request.second < 2) {
				request.second++;
				framebufferWarmQueue_.push_back(request);
			}
			else {
				completed_++;
				framebufferCompleted_++;
				if (result != FramebufferWarmResult::READY)
					failed_++;
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
	double averageConstructionMs() const { return constructedCount_ > 0 ? constructionTotalMs_ / constructedCount_ : 0.0; }
	double maximumConstructionMs() const { return constructionMaxMs_; }

private:
	void finishIfComplete() {
		if (state_ != deepcache::CacheState::WARMING || constructionCompleted_ < constructionTarget_ ||
		    worker_.pendingRequestCount(activeGeneration_) != 0 || completed_ < total_ ||
		    (framebufferWarmForGeneration_ && !framebufferWarmQueue_.empty()))
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
		module_->constructionCompletedCount.store(constructionCompleted_, std::memory_order_relaxed);
		module_->constructionTotalCount.store(constructionTarget_, std::memory_order_relaxed);
		module_->framebufferCompletedCount.store(framebufferCompleted_, std::memory_order_relaxed);
		module_->framebufferTotalCount.store(framebufferTarget_, std::memory_order_relaxed);
	}

	DeepcacheModule* module_ = nullptr;
	DeepcacheBrowser* browser_ = nullptr;
	deepcache::PreviewPlannerWorker worker_;
	deepcache::MemoryPreviewCacheBackend backend_;
	deepcache::CacheState state_ = deepcache::CacheState::IDLE;
	std::uint64_t nextGeneration_ = 0;
	std::uint64_t activeGeneration_ = 0;
	int completed_ = 0;
	int total_ = 0;
	int failed_ = 0;
	int constructionTarget_ = 0;
	int constructionCompleted_ = 0;
	int constructionFailed_ = 0;
	int framebufferTarget_ = 0;
	int framebufferCompleted_ = 0;
	bool framebufferWarmForGeneration_ = false;
	std::deque<std::pair<std::size_t, int>> framebufferWarmQueue_;
	std::unordered_set<std::size_t> generationResidentIndices_;
	double warmingStartedAt_ = 0.0;
	double constructionTotalMs_ = 0.0;
	double constructionMaxMs_ = 0.0;
	int constructedCount_ = 0;
	bool stopped_ = false;
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

struct DeepcacheBrowserOverlay : ui::MenuOverlay {
	widget::Widget* previousBrowser = nullptr;
	DeepcacheBrowser* browser = nullptr;
	bool installed = false;
	bool retired = false;
	bool ownershipConflict = false;

	explicit DeepcacheBrowserOverlay(PreviewCacheManager* cacheManager) {
		bgColor = nvgRGBAf(0.f, 0.f, 0.f, 0.33f);
		if (!APP || !APP->scene)
			return;
		previousBrowser = APP->scene->browser;
		if (previousBrowser) {
			previousBrowser->hide();
			if (previousBrowser->parent == APP->scene)
				APP->scene->removeChild(previousBrowser);
			releaseFramebuffers(previousBrowser);
		}
		browser = new DeepcacheBrowser(cacheManager);
		addChild(browser);
		APP->scene->browser = this;
		APP->scene->addChild(this);
		hide();
		installed = true;
	}

	bool ownsBrowserSlot() const {
		return installed && APP && APP->scene && APP->scene->browser == this;
	}

	void restore() {
		if (!ownsBrowserSlot()) {
			ownershipConflict = installed;
			return;
		}
		APP->scene->browser = previousBrowser;
		if (previousBrowser && !previousBrowser->parent) {
			APP->scene->addChild(previousBrowser);
			previousBrowser->hide();
		}
		if (parent == APP->scene)
			APP->scene->removeChild(this);
		installed = false;
	}

	void retireForSuccessor() {
		ownershipConflict = true;
		retired = true;
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
		if (retired && APP && APP->scene && APP->scene->browser == this) {
			APP->scene->browser = previousBrowser;
			if (previousBrowser && !previousBrowser->parent) {
				APP->scene->addChild(previousBrowser);
				previousBrowser->hide();
			}
			if (parent == APP->scene)
				APP->scene->removeChild(this);
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
	else {
		box.size.x = 12.f * RACK_GRID_WIDTH * zoom;
	}
	box.size.y = RACK_GRID_HEIGHT * zoom;
	box.size = box.size.ceil();
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
	if (state != deepcache::PreviewEntryState::RESIDENT || !framebuffer)
		return FramebufferWarmResult::FAILED;
	if (!APP || !APP->window || !APP->window->vg || !APP->window->fbVg)
		return FramebufferWarmResult::RETRY;
	if (hasValidFramebufferImage()) {
		state = deepcache::PreviewEntryState::FRAMEBUFFER_READY;
		return FramebufferWarmResult::READY;
	}

	try {
		framebuffer->step();
		framebuffer->setDirty();
		// Called only by the scene-level warm host during Rack's draw phase.
		// render() builds the texture without compositing it into the scene.
		framebuffer->render();
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
	previewRoot = nullptr;
	zoomWidget = nullptr;
	framebuffer = nullptr;
	moduleWidget = nullptr;
	moduleContainer = nullptr;
	failureReason.clear();
	state = deepcache::PreviewEntryState::EMPTY;
	updateZoom();
}

void DeepcacheModelBox::draw(const DrawArgs& args) {
	if (state == deepcache::PreviewEntryState::EMPTY || state == deepcache::PreviewEntryState::QUEUED) {
		if (cacheManager)
			cacheManager->promote(modelIndex);
		ensurePreviewConstructed();
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
		nvgFontSize(args.vg, 13.f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGBA(255, 120, 145, 255));
		nvgTextBox(args.vg, 8.f, box.size.y * 0.45f, box.size.x - 16.f,
		           model ? model->name.c_str() : "Preview failed", nullptr);
	}

	const float brightness = math::clamp(settings::rackBrightness + 0.2f, 0.f, 1.f);
	nvgGlobalTint(args.vg, nvgRGBAf(brightness, brightness, brightness, 1.f));
	OpaqueWidget::draw(args);
	if (state == deepcache::PreviewEntryState::RESIDENT && hasValidFramebufferImage())
		state = deepcache::PreviewEntryState::FRAMEBUFFER_READY;

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
				e.consume(addedWidget);
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
		menu->addChild(createMenuItem("Retry Deepcache preview", "", [this]() {
			state = deepcache::PreviewEntryState::EMPTY;
			failureReason.clear();
			if (cacheManager)
				cacheManager->promote(modelIndex);
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
	clearButton->box.size.x = 130.f;
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

	std::vector<plugin::Model*> models;
	std::map<plugin::Model*, int> pluginModelOrders;
	for (plugin::Plugin* plugin : plugin::plugins) {
		int pluginModelOrder = 0;
		for (plugin::Model* model : plugin->models) {
			models.push_back(model);
			pluginModelOrders[model] = pluginModelOrder++;
		}
	}
	std::stable_sort(models.begin(), models.end(), [](plugin::Model* a, plugin::Model* b) {
		return std::make_tuple(lowercase(a->plugin->brand), lowercase(a->name), lowercase(a->slug)) <
		       std::make_tuple(lowercase(b->plugin->brand), lowercase(b->name), lowercase(b->slug));
	});

	modelBoxes.reserve(models.size());
	modelDescriptors.reserve(models.size());
	browserRecords.reserve(models.size());
	for (std::size_t index = 0; index < models.size(); ++index) {
		plugin::Model* model = models[index];
		auto* modelBox = new DeepcacheModelBox(model, index, pluginModelOrders[model], manager);
		modelBoxes.push_back(modelBox);
		modelContainer->addChild(modelBox);
		modelDescriptors.push_back({index, model->plugin->slug, model->plugin->version, model->slug,
		                            model->plugin->brand, model->name, model->isFavorite(), model->hidden});
		browserRecords.push_back(makeBrowserRecord(modelBox));
	}
	lastBrowserZoom = settings::browserZoom;
	lastPreferDarkPanels = settings::preferDarkPanels;
	refresh();
}

DeepcacheBrowser::~DeepcacheBrowser() {
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
	for (DeepcacheModelBox* box : modelBoxes) {
		updateBrowserRecord(box);
		const bool visible = deepcache::browserModelMatches(browserRecords[box->modelIndex], filter);
		box->setVisible(visible);
		if (visible) {
			visibleCount++;
			if (cacheManager)
				cacheManager->promote(box->modelIndex);
		}
	}
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

void DeepcacheBrandMenu::step() {
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

void DeepcacheBrandItem::onAction(const ActionEvent& e) {
	browser->brand = browser->brand == brand ? "" : brand;
	browser->refresh();
}

void DeepcacheBrandItem::step() {
	rightText = CHECKMARK(browser->brand == brand);
	ui::MenuItem::step();
}

void DeepcacheBrandButton::onAction(const ActionEvent& e) {
	auto* menu = createMenu<DeepcacheBrandMenu>();
	menu->anchorPos = getAbsoluteOffset(math::Vec(0, box.size.y));
	menu->box.pos = menu->anchorPos;
	menu->box.size.x = box.size.x;
	auto* allItem = new DeepcacheBrandItem;
	allItem->text = string::translate("Browser.allBrands");
	allItem->browser = browser;
	menu->addChild(allItem);
	menu->addChild(new ui::MenuSeparator);
	std::set<std::string, string::CaseInsensitiveCompare> brands;
	for (plugin::Plugin* plugin : plugin::plugins)
		brands.insert(plugin->brand);
	for (const std::string& brand : brands) {
		auto* item = new DeepcacheBrandItem;
		item->text = brand;
		item->brand = brand;
		item->browser = browser;
		menu->addChild(item);
	}
}

void DeepcacheBrandButton::step() {
	text = string::translate("Browser.brand");
	if (!browser->brand.empty())
		text += ": " + browser->brand;
	text = string::ellipsize(text, 20);
	ui::ChoiceButton::step();
}

void DeepcacheTagItem::onAction(const ActionEvent& e) {
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
	rightText = CHECKMARK(tagId < 0 ? browser->tagIds.empty() : browser->tagIds.count(tagId) != 0);
	ui::MenuItem::step();
}

void DeepcacheTagButton::onAction(const ActionEvent& e) {
	ui::Menu* menu = createMenu();
	menu->box.pos = getAbsoluteOffset(math::Vec(0, box.size.y));
	menu->box.size.x = box.size.x;
	auto* allItem = new DeepcacheTagItem;
	allItem->text = string::translate("Browser.allTags");
	allItem->tagId = -1;
	allItem->browser = browser;
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
	text = string::ellipsize(text, 20);
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
	for (int sortId = settings::BROWSER_SORT_UPDATED; sortId <= settings::BROWSER_SORT_RANDOM; ++sortId) {
		menu->addChild(createCheckMenuItem(names[sortId], "",
			[sortId]() { return settings::browserSort == sortId; },
			[this, sortId]() {
				settings::browserSort = static_cast<settings::BrowserSort>(sortId);
				browser->refresh();
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
			? module->framebufferCompletedCount.load(std::memory_order_relaxed)
			: module->constructionCompletedCount.load(std::memory_order_relaxed);
		const int total = !module ? 0 : framebufferStage
			? module->framebufferTotalCount.load(std::memory_order_relaxed)
			: module->constructionTotalCount.load(std::memory_order_relaxed);
		const bool enabled = !framebufferStage ||
			(module && module->experimentalFramebufferWarm.load(std::memory_order_relaxed));
		const float progress = total > 0 ? math::clamp(completed / static_cast<float>(total), 0.f, 1.f) : 0.f;
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.75f, 0.75f, box.size.x - 1.5f, box.size.y - 1.5f, 2.f);
		nvgFillColor(args.vg, nvgRGBA(8, 13, 22, 235));
		nvgFill(args.vg);
		if (enabled && progress > 0.f) {
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
		nvgFontSize(args.vg, 9.f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, color::WHITE);
		const std::string text = !enabled ? "OFF" : std::to_string(static_cast<int>(std::round(progress * 100.f))) + "%";
		nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.52f, text.c_str(), nullptr);
	}
};

struct DeepcacheModuleCountWidget : widget::TransparentWidget {
	DeepcacheModule* module = nullptr;
	explicit DeepcacheModuleCountWidget(DeepcacheModule* module)
		: module(module) {
	}
	void draw(const DrawArgs& args) override {
		const int completed = module ? module->constructionCompletedCount.load(std::memory_order_relaxed) : 0;
		const std::string text = std::to_string(completed);
		nvgFontSize(args.vg, 7.5f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGBA(188, 204, 224, 220));
		nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.5f, text.c_str(), nullptr);
	}
};

struct DeepcacheCacheButton : LEDButton {
	DeepcacheModule* deepcacheModule = nullptr;
	void draw(const DrawArgs& args) override {
		LEDButton::draw(args);
		if (!deepcacheModule)
			return;
		const auto state = static_cast<deepcache::CacheState>(
			deepcacheModule->cacheState.load(std::memory_order_relaxed));
		NVGcolor glow = nvgRGBA(80, 100, 120, 55);
		switch (state) {
			case deepcache::CacheState::PLANNING: glow = nvgRGBA(255, 184, 55, 150); break;
			case deepcache::CacheState::WARMING: glow = nvgRGBA(35, 215, 235, 165); break;
			case deepcache::CacheState::PAUSED: glow = nvgRGBA(150, 95, 255, 165); break;
			case deepcache::CacheState::READY: glow = nvgRGBA(55, 230, 120, 165); break;
			case deepcache::CacheState::ERROR: glow = nvgRGBA(255, 55, 75, 180); break;
			default: break;
		}
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, box.size.x * 0.5f, box.size.y * 0.5f, std::min(box.size.x, box.size.y) * 0.28f);
		nvgFillColor(args.vg, glow);
		nvgFill(args.vg);
	}
};

DeepcacheWidget* gActiveDeepcacheWidget = nullptr;

}  // namespace

DeepcacheModule::DeepcacheModule() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	configButton(CACHE_PARAM, "Cache");
}

void DeepcacheModule::process(const ProcessArgs& args) {
	(void)args;
	const bool high = params[CACHE_PARAM].getValue() >= 0.5f;
	if (high && !cacheButtonHigh_)
		buttonPressSerial.fetch_add(1, std::memory_order_relaxed);
	cacheButtonHigh_ = high;

	const auto state = static_cast<deepcache::CacheState>(cacheState.load(std::memory_order_relaxed));
	lights[PLANNING_LIGHT].setBrightness(state == deepcache::CacheState::PLANNING ? 1.f : 0.f);
	lights[WARMING_LIGHT].setBrightness(state == deepcache::CacheState::WARMING ? 1.f :
	                                    state == deepcache::CacheState::PAUSED ? 0.3f : 0.f);
	lights[READY_LIGHT].setBrightness(state == deepcache::CacheState::READY ? 1.f : 0.f);
	lights[ERROR_LIGHT].setBrightness(state == deepcache::CacheState::ERROR ? 1.f : 0.f);
}

json_t* DeepcacheModule::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "autoStart", json_boolean(autoStart.load(std::memory_order_relaxed)));
	json_object_set_new(root, "uiBudgetMs", json_real(uiBudgetMicros.load(std::memory_order_relaxed) / 1000.0));
	const auto scope = static_cast<deepcache::CacheScope>(cacheScope.load(std::memory_order_relaxed));
	const char* scopeName = scope == deepcache::CacheScope::FAVORITES ? "favorites" :
	                        scope == deepcache::CacheScope::VISIBLE_SEARCH_RESULTS ? "visible" : "all";
	json_object_set_new(root, "cacheScope", json_string(scopeName));
	json_object_set_new(root, "experimentalFramebufferWarm",
	                    json_boolean(experimentalFramebufferWarm.load(std::memory_order_relaxed)));
	return root;
}

void DeepcacheModule::dataFromJson(json_t* root) {
	if (json_t* value = json_object_get(root, "autoStart"))
		autoStart.store(json_boolean_value(value), std::memory_order_relaxed);
	if (json_t* value = json_object_get(root, "uiBudgetMs")) {
		const double budget = std::max(0.5, std::min(8.0, json_number_value(value)));
		uiBudgetMicros.store(static_cast<int>(std::round(budget * 1000.0)), std::memory_order_relaxed);
	}
	if (json_t* value = json_object_get(root, "cacheScope")) {
		const std::string scope = json_string_value(value) ? json_string_value(value) : "all";
		cacheScope.store(static_cast<int>(scope == "favorites" ? deepcache::CacheScope::FAVORITES :
		                                  scope == "visible" ? deepcache::CacheScope::VISIBLE_SEARCH_RESULTS :
		                                                       deepcache::CacheScope::ALL),
		                 std::memory_order_relaxed);
	}
	if (json_t* value = json_object_get(root, "experimentalFramebufferWarm"))
		experimentalFramebufferWarm.store(json_boolean_value(value), std::memory_order_relaxed);
}

struct DeepcacheWidget::Internal {
	DeepcacheModule* module = nullptr;
	PreviewCacheManager* cacheManager = nullptr;
	DeepcacheBrowserOverlay* overlay = nullptr;
	DeepcacheWarmRenderHost* warmRenderHost = nullptr;
	bool active = false;
	bool autoStartPending = false;
	std::uint64_t handledButtonSerial = 0;
};

DeepcacheWidget::DeepcacheWidget(DeepcacheModule* module) {
	setModule(module);
	const std::string panelPath = asset::plugin(pluginInstance, "res/Deepcache.svg");
	setPanel(createPanel(panelPath));

	math::Vec cacheButtonMm(10.16f, 110.f);
	math::Vec planningMm(4.f, 78.f);
	math::Vec warmingMm(8.f, 78.f);
	math::Vec readyMm(12.f, 78.f);
	math::Vec errorMm(16.f, 78.f);
	panel_svg::loadPointFromSvgMm(panelPath, "cache_param", &cacheButtonMm);
	panel_svg::loadPointFromSvgMm(panelPath, "planning_light", &planningMm);
	panel_svg::loadPointFromSvgMm(panelPath, "warming_light", &warmingMm);
	panel_svg::loadPointFromSvgMm(panelPath, "ready_light", &readyMm);
	panel_svg::loadPointFromSvgMm(panelPath, "error_light", &errorMm);

	math::Rect progressRectMm(math::Vec(3.f, 50.f), math::Vec(14.32f, 7.f));
	math::Rect moduleCountRectMm(math::Vec(3.f, 57.2f), math::Vec(14.32f, 4.5f));
	math::Rect framebufferProgressRectMm(math::Vec(3.f, 66.f), math::Vec(14.32f, 7.f));
	panel_svg::loadRectFromSvgMm(panelPath, "progress", &progressRectMm);
	panel_svg::loadRectFromSvgMm(panelPath, "progress_count", &moduleCountRectMm);
	panel_svg::loadRectFromSvgMm(panelPath, "framebuffer_progress", &framebufferProgressRectMm);
	auto* cacheButton = createParamCentered<DeepcacheCacheButton>(
		mm2px(cacheButtonMm), module, DeepcacheModule::CACHE_PARAM);
	cacheButton->deepcacheModule = module;
	addParam(cacheButton);
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
	addChild(createLightCentered<TinyLight<YellowLight>>(mm2px(planningMm), module, DeepcacheModule::PLANNING_LIGHT));
	addChild(createLightCentered<TinyLight<BlueLight>>(mm2px(warmingMm), module, DeepcacheModule::WARMING_LIGHT));
	addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(readyMm), module, DeepcacheModule::READY_LIGHT));
	addChild(createLightCentered<TinyLight<RedLight>>(mm2px(errorMm), module, DeepcacheModule::ERROR_LIGHT));

	// Browser previews must remain completely inert.
	if (!module)
		return;
	internal_ = new Internal;
	internal_->module = module;
	internal_->handledButtonSerial = module->buttonPressSerial.load(std::memory_order_relaxed);
	if (!gActiveDeepcacheWidget) {
		gActiveDeepcacheWidget = this;
		internal_->active = true;
		internal_->cacheManager = new PreviewCacheManager(module);
		internal_->overlay = new DeepcacheBrowserOverlay(internal_->cacheManager);
		if (internal_->overlay->installed && internal_->overlay->browser) {
			internal_->cacheManager->setBrowser(internal_->overlay->browser);
			internal_->warmRenderHost = new DeepcacheWarmRenderHost;
			internal_->warmRenderHost->cacheManager = internal_->cacheManager;
			APP->scene->addChild(internal_->warmRenderHost);
			internal_->autoStartPending = module->autoStart.load(std::memory_order_relaxed);
		}
		else {
			module->cacheState.store(static_cast<int>(deepcache::CacheState::ERROR), std::memory_order_relaxed);
		}
	}
	else {
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
		gActiveDeepcacheWidget = nullptr;
	}
	delete internal_;
	internal_ = nullptr;
}

void DeepcacheWidget::step() {
	if (internal_ && internal_->active && internal_->cacheManager) {
		if (internal_->autoStartPending) {
			internal_->autoStartPending = false;
			internal_->cacheManager->start();
		}
		const std::uint64_t serial = internal_->module->buttonPressSerial.load(std::memory_order_relaxed);
		if (serial != internal_->handledButtonSerial) {
			internal_->handledButtonSerial = serial;
			internal_->cacheManager->handleButtonPress();
		}
		internal_->cacheManager->step();
	}
	ModuleWidget::step();
}

void DeepcacheWidget::appendContextMenu(ui::Menu* menu) {
	ModuleWidget::appendContextMenu(menu);
	if (!internal_ || !internal_->module)
		return;
	menu->addChild(new ui::MenuSeparator);
	if (!internal_->active) {
		menu->addChild(createMenuLabel("Passive: another Deepcache owns the browser"));
		return;
	}
	PreviewCacheManager* manager = internal_->cacheManager;
	menu->addChild(createMenuItem("Start cache", "", [manager]() { manager->start(); }));
	menu->addChild(createMenuItem("Pause cache", "", [manager]() { manager->pause(); }));
	menu->addChild(createMenuItem("Resume cache", "", [manager]() { manager->resume(); }));
	menu->addChild(createMenuItem("Cancel cache", "", [manager]() { manager->cancel(); }));
	menu->addChild(createMenuItem("Clear memory cache", "", [manager]() { manager->clear(); }));
	menu->addChild(createMenuItem("Rebuild cache", "", [manager]() { manager->rebuild(); }));
	menu->addChild(new ui::MenuSeparator);
	DeepcacheModule* deepcacheModule = internal_->module;
	menu->addChild(createCheckMenuItem("Auto-start on patch load", "",
		[deepcacheModule]() { return deepcacheModule->autoStart.load(std::memory_order_relaxed); },
		[deepcacheModule]() {
			const bool value = deepcacheModule->autoStart.load(std::memory_order_relaxed);
			deepcacheModule->autoStart.store(!value, std::memory_order_relaxed);
		}));
	menu->addChild(createSubmenuItem("UI work budget", "", [deepcacheModule](ui::Menu* child) {
		for (int micros : {500, 1000, 2000, 4000, 8000}) {
			const std::string label = micros < 1000 ? "0.5 ms" : std::to_string(micros / 1000) + " ms";
			child->addChild(createCheckMenuItem(label, "",
				[deepcacheModule, micros]() { return deepcacheModule->uiBudgetMicros.load(std::memory_order_relaxed) == micros; },
				[deepcacheModule, micros]() { deepcacheModule->uiBudgetMicros.store(micros, std::memory_order_relaxed); }));
		}
	}));
	menu->addChild(createSubmenuItem("Cache scope", "", [deepcacheModule](ui::Menu* child) {
		const std::pair<const char*, deepcache::CacheScope> scopes[] = {
			{"Favorites", deepcache::CacheScope::FAVORITES},
			{"Visible search results", deepcache::CacheScope::VISIBLE_SEARCH_RESULTS},
			{"All installed modules", deepcache::CacheScope::ALL},
		};
		for (const auto& scope : scopes) {
			child->addChild(createCheckMenuItem(scope.first, "",
				[deepcacheModule, scope]() { return deepcacheModule->cacheScope.load(std::memory_order_relaxed) == static_cast<int>(scope.second); },
				[deepcacheModule, scope]() { deepcacheModule->cacheScope.store(static_cast<int>(scope.second), std::memory_order_relaxed); }));
		}
	}));
	menu->addChild(createCheckMenuItem("Experimental framebuffer warm pass", "",
		[deepcacheModule]() { return deepcacheModule->experimentalFramebufferWarm.load(std::memory_order_relaxed); },
		[deepcacheModule, manager]() {
			const bool value = deepcacheModule->experimentalFramebufferWarm.load(std::memory_order_relaxed);
			manager->setFramebufferWarmEnabled(!value);
		}));
	menu->addChild(createMenuLabel("Uses additional GPU memory; applied immediately"));
	menu->addChild(createSubmenuItem("Cache statistics", "", [manager](ui::Menu* child) {
		child->addChild(createMenuLabel("Completed: " + std::to_string(manager->completedCount()) +
		                                    " / " + std::to_string(manager->totalCount())));
		child->addChild(createMenuLabel("Resident records: " + std::to_string(manager->residentRecordCount())));
		child->addChild(createMenuLabel("Resident previews: " + std::to_string(manager->residentPreviewCount())));
		child->addChild(createMenuLabel("Framebuffer-ready: " + std::to_string(manager->framebufferReadyPreviewCount())));
		child->addChild(createMenuLabel("Failed: " + std::to_string(manager->failedCount())));
		child->addChild(createMenuLabel(string::f("Average construction: %.2f ms", manager->averageConstructionMs())));
		child->addChild(createMenuLabel(string::f("Maximum construction: %.2f ms", manager->maximumConstructionMs())));
	}));
	if (internal_->overlay && internal_->overlay->ownershipConflict)
		menu->addChild(createMenuLabel("Warning: another plugin currently owns the browser"));
}

Model* modelDeepcache = createModel<DeepcacheModule, DeepcacheWidget>("Deepcache");
