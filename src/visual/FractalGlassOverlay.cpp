#include "FractalGlassOverlay.hpp"

#include "../NvgGraphicsLifecycle.hpp"
#include "../PanelSvgUtils.hpp"

#include <atomic>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <future>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace visual_assets {
namespace {

constexpr int kReferenceRenderHeight = 480;
constexpr int kReferenceRenderWidth = 384;
constexpr int kReferencePanelHp = 8;
constexpr int kMaxRenderWidth = 1024;
constexpr size_t kMaxCachedFractalFields = 32u;
constexpr float kFractalGlassOpacity = 0.22f;
constexpr float kFractalGlassContrast = 1.35f;

std::string libraryPath() {
	if (isDragonKingUserFractalParamsEnabled()) {
		return system::join(asset::user(), "Leviathan/IntegralFlux/FractalParams.json");
	}
	return pluginInstance
		? asset::plugin(pluginInstance, "res/FractalParams.json")
		: std::string();
}

bool sameParams(const iris::NautiloidFractalSourceParams& a,
	const iris::NautiloidFractalSourceParams& b) {
	return a.mode == b.mode && std::fabs(a.zoom - b.zoom) <= 1e-5f &&
		std::fabs(a.centerX - b.centerX) <= 1e-7 &&
		std::fabs(a.centerY - b.centerY) <= 1e-7;
}

iris::FractalPalette paletteForColor(NVGcolor color) {
	const int r = int(std::round(clamp(color.r, 0.f, 1.f) * 255.f));
	const int g = int(std::round(clamp(color.g, 0.f, 1.f) * 255.f));
	const int b = int(std::round(clamp(color.b, 0.f, 1.f) * 255.f));
	iris::FractalPalette palette;
	palette.shadowR = uint8_t(std::max(1, r * 7 / 12));
	palette.shadowG = uint8_t(std::max(1, g * 7 / 12));
	palette.shadowB = uint8_t(std::max(1, b * 7 / 12));
	palette.highlightR = uint8_t(std::min(255, r + (255 - r) / 3));
	palette.highlightG = uint8_t(std::min(255, g + (255 - g) / 3));
	palette.highlightB = uint8_t(std::min(255, b + (255 - b) / 3));
	palette.contrast = kFractalGlassContrast;
	return palette;
}

struct SavedEntry {
	iris::NautiloidFractalSourceParams params;
	size_t index = 0u;
};

struct SelectionPool {
	uint64_t librarySignature = 0u;
	std::vector<size_t> shuffledEntries;
	size_t nextEntry = 0u;
};

std::mutex gSelectionPoolMutex;
std::unordered_map<std::string, SelectionPool> gSelectionPools;

struct FractalRenderKey {
	int width = 0;
	int height = 0;
	int mode = iris::FRACTAL_NONE;
	uint32_t zoomBits = 0u;
	uint64_t centerXBits = 0u;
	uint64_t centerYBits = 0u;

	bool operator==(const FractalRenderKey& other) const {
		return width == other.width && height == other.height && mode == other.mode &&
			zoomBits == other.zoomBits && centerXBits == other.centerXBits &&
			centerYBits == other.centerYBits;
	}
};

struct FractalRenderKeyHash {
	size_t operator()(const FractalRenderKey& key) const {
		size_t hash = size_t(1469598103934665603ull);
		auto mix = [&](uint64_t value) {
			hash ^= size_t(value ^ (value >> 32u));
			hash *= size_t(1099511628211ull);
		};
		mix(uint64_t(uint32_t(key.width)) | (uint64_t(uint32_t(key.height)) << 32u));
		mix(uint64_t(uint32_t(key.mode)) | (uint64_t(key.zoomBits) << 32u));
		mix(key.centerXBits);
		mix(key.centerYBits);
		return hash;
	}
};

using CachedFractalField = std::shared_ptr<const iris::SourceField>;
using CachedFractalFuture = std::shared_future<CachedFractalField>;
std::mutex gFractalRenderCacheMutex;
std::unordered_map<FractalRenderKey, CachedFractalFuture, FractalRenderKeyHash>
	gFractalRenderCache;

FractalRenderKey makeFractalRenderKey(
	const iris::NautiloidFractalSourceParams& params, int width, int height) {
	FractalRenderKey key;
	key.width = width;
	key.height = height;
	key.mode = params.mode;
	std::memcpy(&key.zoomBits, &params.zoom, sizeof(key.zoomBits));
	std::memcpy(&key.centerXBits, &params.centerX, sizeof(key.centerXBits));
	std::memcpy(&key.centerYBits, &params.centerY, sizeof(key.centerYBits));
	return key;
}

CachedFractalField renderFractalField(
	const iris::NautiloidFractalSourceParams& params, int width, int height) {
	std::shared_ptr<iris::SourceField> source(new iris::SourceField);
	if (!iris::makeBuiltinFractalSourceSized(params.mode, params.zoom,
		params.centerX, params.centerY, width, height, 1.f, source.get()) ||
		!source->valid()) {
		return CachedFractalField();
	}
	return source;
}

CachedFractalField cachedFractalField(
	const iris::NautiloidFractalSourceParams& params, int width, int height) {
	const FractalRenderKey key = makeFractalRenderKey(params, width, height);
	CachedFractalFuture future;
	std::shared_ptr<std::promise<CachedFractalField>> producer;
	{
		std::lock_guard<std::mutex> lock(gFractalRenderCacheMutex);
		const auto existing = gFractalRenderCache.find(key);
		if (existing != gFractalRenderCache.end()) {
			future = existing->second;
		} else {
			if (gFractalRenderCache.size() >= kMaxCachedFractalFields) {
				for (auto it = gFractalRenderCache.begin();
					it != gFractalRenderCache.end() &&
					gFractalRenderCache.size() >= kMaxCachedFractalFields;) {
					if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
						it = gFractalRenderCache.erase(it);
					} else {
						++it;
					}
				}
			}
			producer.reset(new std::promise<CachedFractalField>);
			future = producer->get_future().share();
			gFractalRenderCache.emplace(key, future);
		}
	}
	if (producer) {
		producer->set_value(renderFractalField(params, width, height));
	}
	return future.get();
}

uint64_t savedEntriesSignature(const std::vector<SavedEntry>& entries) {
	uint64_t hash = 1469598103934665603ull;
	auto mix = [&](uint64_t value) {
		hash ^= value;
		hash *= 1099511628211ull;
	};
	for (const SavedEntry& entry : entries) {
		mix(uint64_t(entry.index));
		mix(uint64_t(uint32_t(entry.params.mode)));
		uint32_t zoomBits = 0u;
		uint64_t centerXBits = 0u;
		uint64_t centerYBits = 0u;
		std::memcpy(&zoomBits, &entry.params.zoom, sizeof(zoomBits));
		std::memcpy(&centerXBits, &entry.params.centerX, sizeof(centerXBits));
		std::memcpy(&centerYBits, &entry.params.centerY, sizeof(centerYBits));
		mix(zoomBits);
		mix(centerXBits);
		mix(centerYBits);
	}
	return hash;
}

void shuffleEntryOrder(std::vector<size_t>* order) {
	if (!order) return;
	for (size_t i = order->size(); i > 1u; --i) {
		const size_t swapIndex = size_t(random::u32()) % i;
		std::swap((*order)[i - 1u], (*order)[swapIndex]);
	}
}

bool selectRandomEntry(const std::string& selectionKey,
	iris::NautiloidFractalSourceParams* params, size_t* entryIndex) {
	if (!params || !entryIndex) return false;
	json_error_t error;
	json_t* root = json_load_file(libraryPath().c_str(), 0, &error);
	json_t* entries = root && json_is_object(root) ? json_object_get(root, "entries") : nullptr;
	if (!entries || !json_is_array(entries)) {
		if (root) json_decref(root);
		return false;
	}
	std::vector<SavedEntry> valid;
	for (size_t i = 0; i < json_array_size(entries); ++i) {
		json_t* entry = json_array_get(entries, i);
		json_t* fractal = entry && json_is_object(entry) ? json_object_get(entry, "fractal") : nullptr;
		json_t* mode = fractal ? json_object_get(fractal, "mode") : nullptr;
		json_t* zoom = fractal ? json_object_get(fractal, "zoom") : nullptr;
		json_t* x = fractal ? json_object_get(fractal, "centerX") : nullptr;
		json_t* y = fractal ? json_object_get(fractal, "centerY") : nullptr;
		if (!json_is_integer(mode) || !json_is_number(zoom) ||
			!json_is_number(x) || !json_is_number(y)) continue;
		SavedEntry candidate;
		candidate.params.mode = int(json_integer_value(mode));
		candidate.params.zoom = float(json_number_value(zoom));
		candidate.params.centerX = json_number_value(x);
		candidate.params.centerY = json_number_value(y);
		candidate.index = i;
		if (!iris::isBuiltinFractalMode(candidate.params.mode) ||
			!std::isfinite(candidate.params.zoom) || !std::isfinite(candidate.params.centerX) ||
			!std::isfinite(candidate.params.centerY)) continue;
		candidate.params.zoom = clamp(candidate.params.zoom, 0.f, 4.f);
		candidate.params.centerX = std::max(-2.0, std::min(candidate.params.centerX, 2.0));
		candidate.params.centerY = std::max(-2.0, std::min(candidate.params.centerY, 2.0));
		valid.push_back(candidate);
	}
	json_decref(root);
	if (valid.empty()) return false;
	const uint64_t signature = savedEntriesSignature(valid);
	std::lock_guard<std::mutex> poolLock(gSelectionPoolMutex);
	SelectionPool& pool = gSelectionPools[selectionKey];
	if (pool.librarySignature != signature || pool.nextEntry >= pool.shuffledEntries.size()) {
		pool.librarySignature = signature;
		pool.shuffledEntries.resize(valid.size());
		for (size_t i = 0; i < valid.size(); ++i) pool.shuffledEntries[i] = i;
		shuffleEntryOrder(&pool.shuffledEntries);
		pool.nextEntry = 0u;
	}
	const SavedEntry& selected = valid[pool.shuffledEntries[pool.nextEntry++]];
	*params = selected.params;
	*entryIndex = selected.index;
	return true;
}

bool deleteEntry(size_t index) {
	// The bundled library is installation data. Only the explicitly selected
	// user library may be edited by Integral Flux's debug menu.
	if (!isDragonKingUserFractalParamsEnabled()) return false;
	json_error_t error;
	const std::string path = libraryPath();
	json_t* root = json_load_file(path.c_str(), 0, &error);
	json_t* entries = root && json_is_object(root) ? json_object_get(root, "entries") : nullptr;
	if (!entries || !json_is_array(entries) || index >= json_array_size(entries)) {
		if (root) json_decref(root);
		return false;
	}
	json_array_remove(entries, index);
	const int result = json_dump_file(root, path.c_str(), JSON_INDENT(2) | JSON_SORT_KEYS);
	json_decref(root);
	return result == 0;
}

} // namespace

struct FractalGlassOverlay::Impl {
	struct Region {
		math::Rect rect;
		float radius = 0.f;
		std::vector<panel_svg::SvgPathCommand> path;
		iris::FractalPalette palette;
	};
	struct Request {
		iris::NautiloidFractalSourceParams params;
		uint64_t serial = 0u;
		bool cacheable = false;
	};
	widget::FramebufferWidget* framebuffer = nullptr;
	std::vector<Region> regions;
	std::vector<int> images;
	std::vector<uint8_t> rgba;
	NVGcontext* imageContext = nullptr;
	int uploadedWidth = 0;
	int uploadedHeight = 0;
	uint64_t uploadedGeneration = uint64_t(-1);
	uint64_t observedGeneration = uint64_t(-1);
	std::mutex requestMutex;
	std::condition_variable requestCv;
	bool stop = false;
	bool pending = false;
	Request request;
	uint64_t nextSerial = 0u;
	std::thread worker;
	std::mutex resultMutex;
	std::vector<std::vector<uint8_t>> rendered;
	int renderedWidth = 0;
	int renderedHeight = 0;
	std::atomic<uint64_t> generation {0u};
	bool liveValid = false;
	iris::NautiloidFractalSourceParams live;
	bool wasLive = false;
	bool fallbackAttempted = false;
	bool fallbackValid = false;
	iris::NautiloidFractalSourceParams fallback;
	size_t fallbackIndex = size_t(-1);
	bool submittedValid = false;
	iris::NautiloidFractalSourceParams submitted;
	std::string selectionKey;
	int renderWidth = 2;
	int renderHeight = kReferenceRenderHeight;

	Impl(const std::string& panelPath, const std::string& requestedSelectionKey,
		int requestedRenderWidth, int requestedRenderHeight,
		bool synchronousFallback)
		: selectionKey(requestedSelectionKey)
		, renderWidth(std::max(2, requestedRenderWidth))
		, renderHeight(std::max(2, requestedRenderHeight)) {
		std::vector<panel_svg::SvgRectMatch> matches;
		if (panel_svg::findRectsInGroupsWithIdSubstringMm(panelPath, "glass", &matches)) {
			for (const auto& match : matches) {
				Region region;
				region.rect = math::Rect(mm2px(match.rect.pos), mm2px(match.rect.size));
				if (match.hasCornerRadius) { Vec r = mm2px(match.cornerRadius); region.radius = std::min(r.x, r.y); }
				region.palette = paletteForColor(match.hasFillColor ? match.fillColor : nvgRGB(124, 92, 255));
				regions.push_back(region);
			}
		}
		std::vector<panel_svg::SvgPathMatch> pathMatches;
		if (panel_svg::findPathsInGroupsWithIdSubstringMm(panelPath, "glass", &pathMatches)) {
			for (const auto& match : pathMatches) {
				Region region;
				region.rect = math::Rect(mm2px(match.bounds.pos), mm2px(match.bounds.size));
				region.path.reserve(match.commands.size());
				for (panel_svg::SvgPathCommand command : match.commands) {
					command.p1 = mm2px(command.p1);
					command.p2 = mm2px(command.p2);
					command.p3 = mm2px(command.p3);
					region.path.push_back(command);
				}
				region.palette = paletteForColor(
					match.hasFillColor ? match.fillColor : nvgRGB(124, 92, 255));
				regions.push_back(region);
			}
		}
		images.assign(regions.size(), -1);
		if (synchronousFallback && !regions.empty()) {
			fallbackValid = selectRandomEntry(
				selectionKey, &fallback, &fallbackIndex);
			fallbackAttempted = true;
			if (fallbackValid) {
				submitted = fallback;
				submittedValid = true;
				publishRendered(cachedFractalField(
					fallback, renderWidth, renderHeight));
			}
		}
		worker = std::thread([this]() { loop(); });
	}

	~Impl() {
		{ std::lock_guard<std::mutex> lock(requestMutex); stop = true; pending = false; }
		requestCv.notify_one();
		if (worker.joinable()) worker.join();
	}

	void submit(const iris::NautiloidFractalSourceParams& params, bool cacheable) {
		std::lock_guard<std::mutex> lock(requestMutex);
		request.params = params;
		request.serial = ++nextSerial;
		request.cacheable = cacheable;
		pending = true;
		requestCv.notify_one();
	}

	void publishRendered(const CachedFractalField& source) {
		if (!source || !source->valid()) return;
		{
			std::lock_guard<std::mutex> resultLock(resultMutex);
			rendered.resize(regions.size());
			for (size_t i = 0; i < regions.size(); ++i) {
				iris::SourceField tinted = *source;
				iris::applyFractalPalette(&tinted, regions[i].palette);
				rendered[i] = std::move(tinted.rgb8);
			}
			renderedWidth = source->width;
			renderedHeight = source->height;
		}
		generation.fetch_add(1u, std::memory_order_release);
	}

	void loop() {
		while (true) {
			Request current;
			{
				std::unique_lock<std::mutex> lock(requestMutex);
				requestCv.wait(lock, [this]() { return stop || pending; });
				if (stop) return;
				current = request;
				pending = false;
			}
			const CachedFractalField source = current.cacheable
				? cachedFractalField(current.params, renderWidth, renderHeight)
				: renderFractalField(current.params, renderWidth, renderHeight);
			if (!source || !source->valid()) continue;
			std::lock_guard<std::mutex> requestLock(requestMutex);
			if (current.serial != nextSerial) continue;
			publishRendered(source);
		}
	}
};

FractalGlassOverlay::FractalGlassOverlay(
	const std::string& panelPath, const std::string& selectionKey,
	int renderWidth, int renderHeight, bool synchronousFallback)
	: impl(new Impl(panelPath, selectionKey, renderWidth, renderHeight,
		synchronousFallback)) {}

FractalGlassOverlay::~FractalGlassOverlay() = default;

void FractalGlassOverlay::setFramebuffer(widget::FramebufferWidget* framebuffer) { impl->framebuffer = framebuffer; }

void FractalGlassOverlay::setLiveParams(const iris::NautiloidFractalSourceParams* params) {
	impl->liveValid = params != nullptr;
	if (params) impl->live = *params;
}

bool FractalGlassOverlay::isReadyForCapture() const {
	if (impl->regions.empty()) return true;
	if (!impl->liveValid && impl->fallbackAttempted && !impl->fallbackValid) {
		return true;
	}
	return impl->generation.load(std::memory_order_acquire) > 0u;
}

bool FractalGlassOverlay::hasFallbackSelection() const {
	return isDragonKingUserFractalParamsEnabled() &&
		!impl->liveValid && impl->fallbackValid && impl->fallbackIndex != size_t(-1);
}

bool FractalGlassOverlay::deleteFallbackSelection() {
	if (!hasFallbackSelection() || !deleteEntry(impl->fallbackIndex)) return false;
	{
		std::lock_guard<std::mutex> requestLock(impl->requestMutex);
		impl->pending = false;
		++impl->nextSerial;
	}
	{
		std::lock_guard<std::mutex> resultLock(impl->resultMutex);
		impl->rendered.clear();
		impl->renderedWidth = 0;
		impl->renderedHeight = 0;
	}
	impl->fallbackValid = false;
	impl->fallbackIndex = size_t(-1);
	impl->fallbackAttempted = false;
	impl->submittedValid = false;
	impl->generation.fetch_add(1u, std::memory_order_release);
	return true;
}

void FractalGlassOverlay::step() {
	if (impl->liveValid) {
		if (!impl->submittedValid || !sameParams(impl->live, impl->submitted)) {
			impl->submitted = impl->live;
			impl->submittedValid = true;
			impl->submit(impl->live, false);
		}
		impl->fallbackAttempted = false;
	} else {
		if (impl->wasLive) impl->fallbackAttempted = false;
		if (!impl->fallbackAttempted) {
			impl->fallbackValid = selectRandomEntry(
				impl->selectionKey, &impl->fallback, &impl->fallbackIndex);
			impl->fallbackAttempted = true;
		}
		if (impl->fallbackValid && (!impl->submittedValid || !sameParams(impl->fallback, impl->submitted))) {
			impl->submitted = impl->fallback;
			impl->submittedValid = true;
			impl->submit(impl->fallback, true);
		}
	}
	impl->wasLive = impl->liveValid;
	const uint64_t generation = impl->generation.load(std::memory_order_acquire);
	if (impl->framebuffer && generation != impl->observedGeneration) impl->framebuffer->setDirty();
	impl->observedGeneration = generation;
	TransparentWidget::step();
}

void FractalGlassOverlay::draw(const DrawArgs& args) {
	if (impl->regions.empty()) return;
	auto resetImages = [&](bool deleteHandles) {
		if (deleteHandles && impl->imageContext == args.vg) for (int image : impl->images) if (image >= 0) nvgDeleteImage(args.vg, image);
		impl->images.assign(impl->regions.size(), -1);
		impl->uploadedWidth = impl->uploadedHeight = 0;
	};
	if (impl->imageContext != args.vg) {
		resetImages(false);
		impl->imageContext = args.vg;
		impl->uploadedGeneration = uint64_t(-1);
	}
	bool valid = impl->images.size() == impl->regions.size() && impl->uploadedWidth > 0;
	if (valid) for (int image : impl->images) if (!nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg, image, impl->uploadedWidth, impl->uploadedHeight)) { valid = false; break; }
	const uint64_t generation = impl->generation.load(std::memory_order_acquire);
	if (generation != impl->uploadedGeneration || !valid) {
		std::vector<std::vector<uint8_t>> rendered;
		int width = 0, height = 0;
		{
			std::lock_guard<std::mutex> lock(impl->resultMutex);
			rendered = impl->rendered;
			width = impl->renderedWidth;
			height = impl->renderedHeight;
		}
		resetImages(true);
		if (width > 0 && height > 0 && rendered.size() == impl->regions.size()) {
			for (size_t region = 0; region < rendered.size(); ++region) {
				const auto& rgb = rendered[region];
				impl->rgba.resize(rgb.size() / 3u * 4u);
				for (size_t i = 0; i + 2u < rgb.size(); i += 3u) { size_t o = i / 3u * 4u; impl->rgba[o] = rgb[i]; impl->rgba[o + 1u] = rgb[i + 1u]; impl->rgba[o + 2u] = rgb[i + 2u]; impl->rgba[o + 3u] = 255u; }
				impl->images[region] = nvgCreateImageRGBA(args.vg, width, height, NVG_IMAGE_PREMULTIPLIED, impl->rgba.data());
			}
			impl->uploadedWidth = width;
			impl->uploadedHeight = height;
		}
		impl->uploadedGeneration = generation;
	}
	for (size_t i = 0; i < impl->regions.size(); ++i) {
		if (impl->images[i] < 0) continue;
		const auto& region = impl->regions[i];
		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		if (region.path.empty()) {
			nvgScissor(args.vg, region.rect.pos.x, region.rect.pos.y,
				region.rect.size.x, region.rect.size.y);
			nvgRoundedRect(args.vg, region.rect.pos.x, region.rect.pos.y,
				region.rect.size.x, region.rect.size.y, region.radius);
		} else {
			for (const panel_svg::SvgPathCommand& command : region.path) {
				switch (command.type) {
					case panel_svg::SvgPathCommand::MoveTo:
						nvgMoveTo(args.vg, command.p1.x, command.p1.y);
						break;
					case panel_svg::SvgPathCommand::LineTo:
						nvgLineTo(args.vg, command.p1.x, command.p1.y);
						break;
					case panel_svg::SvgPathCommand::QuadTo:
						nvgQuadTo(args.vg, command.p1.x, command.p1.y,
							command.p2.x, command.p2.y);
						break;
					case panel_svg::SvgPathCommand::BezierTo:
						nvgBezierTo(args.vg, command.p1.x, command.p1.y,
							command.p2.x, command.p2.y, command.p3.x, command.p3.y);
						break;
					case panel_svg::SvgPathCommand::Close:
						nvgClosePath(args.vg);
						break;
				}
			}
		}
		nvgFillPaint(args.vg, nvgImagePattern(
			args.vg, 0.f, 0.f, box.size.x, box.size.y,
			0.f, impl->images[i], kFractalGlassOpacity));
		nvgFill(args.vg);
		nvgRestore(args.vg);
	}
}

FractalGlassOverlay* addFractalGlassOverlay(
	ModuleWidget* parent, const std::string& panelPath, Widget* upperSibling) {
	if (!parent) return nullptr;
	auto* framebuffer = new widget::FramebufferWidget();
	framebuffer->box.size = parent->box.size;
	framebuffer->dirtyOnSubpixelChange = false;
	const std::string selectionKey = panelPath + (parent->module ? "" : "#preview");
	const int renderHeight = kReferenceRenderHeight;
	const float referencePanelWidth = float(kReferencePanelHp * RACK_GRID_WIDTH);
	const int renderWidth = clamp(int(std::round(
		float(kReferenceRenderWidth) * parent->box.size.x / referencePanelWidth)),
		2, kMaxRenderWidth);
	const bool modulePreview = parent->module == nullptr;
	auto* overlay = new FractalGlassOverlay(
		panelPath, selectionKey, renderWidth, renderHeight, modulePreview);
	overlay->box.size = parent->box.size;
	overlay->setFramebuffer(framebuffer);
	framebuffer->addChild(overlay);
	if (upperSibling && parent->hasChild(upperSibling)) {
		parent->addChildBelow(framebuffer, upperSibling);
	}
	else {
		parent->addChild(framebuffer);
	}
	return overlay;
}

} // namespace visual_assets
