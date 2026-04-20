#include "CrownstepShared.hpp"
#include "PanelSvgUtils.hpp"

#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>
#include <cmath>

namespace {

constexpr int CHESS_ATLAS_ROWS = 2;
constexpr int CHESS_ATLAS_COLS = 6;
constexpr int HIGHLIGHT_COLOR_COUNT = 3;
constexpr bool CHESS_ATLAS_ENABLED = true;
constexpr float CHESS_HORIZONTAL_SCALE = 1.08f;
constexpr float CHESS_ATLAS_RASTER_SCALE = 3.f;
constexpr unsigned char CHESS_MASK_SOLID_ALPHA_THRESHOLD = 24u;
// Keep alpha neutral to avoid edge over-brightening halos on thin contours.
constexpr float CHESS_ATLAS_ALPHA_GAMMA = 1.f;
// Disable mipmaps for packed chess atlas to reduce edge bleed/fringe at glyph borders.
constexpr int CHESS_ATLAS_IMAGE_FLAGS = 0;
constexpr int CHESS_ATLAS_MASK_FLAGS = 0;
constexpr bool CHESS_ATLAS_SCALE_PER_COLOR = false;

struct HighlightPalette {
	uint8_t shellR;
	uint8_t shellG;
	uint8_t shellB;
	uint8_t bandR;
	uint8_t bandG;
	uint8_t bandB;
	uint8_t glowR;
	uint8_t glowG;
	uint8_t glowB;
};

constexpr HighlightPalette HIGHLIGHT_PALETTES[HIGHLIGHT_COLOR_COUNT] = {
	{112u, 72u, 184u, 198u, 150u, 255u, 184u, 132u, 255u},
	{26u, 178u, 214u, 110u, 232u, 255u, 96u, 222u, 248u},
	{40u, 168u, 104u, 98u, 235u, 154u, 88u, 240u, 154u},
};

inline int highlightPaletteIndexForMode(int highlightMode) {
	switch (highlightMode) {
	case Crownstep::HIGHLIGHT_PURPLE:
		return 0;
	case Crownstep::HIGHLIGHT_CYAN:
		return 1;
	case Crownstep::HIGHLIGHT_GREEN:
	default:
		return 2;
	}
}

inline HighlightPalette highlightPaletteForMode(int highlightMode) {
	return HIGHLIGHT_PALETTES[highlightPaletteIndexForMode(highlightMode)];
}

inline NVGcolor highlightShellColor(int highlightMode, int alpha) {
	const HighlightPalette palette = highlightPaletteForMode(highlightMode);
	return nvgRGBA(palette.shellR, palette.shellG, palette.shellB, alpha);
}

inline NVGcolor highlightBandColor(int highlightMode, int alpha) {
	const HighlightPalette palette = highlightPaletteForMode(highlightMode);
	return nvgRGBA(palette.bandR, palette.bandG, palette.bandB, alpha);
}

inline NVGcolor highlightGlowColor(int highlightMode, int alpha) {
	const HighlightPalette palette = highlightPaletteForMode(highlightMode);
	return nvgRGBA(palette.glowR, palette.glowG, palette.glowB, alpha);
}

struct BananutBlack : app::SvgPort {
	BananutBlack() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg")));
	}
};

std::shared_ptr<Image> crownstepWoodBoardTileImage() {
	if (!APP || !APP->window) {
		return std::shared_ptr<Image>();
	}
	return APP->window->loadImage(asset::plugin(pluginInstance, "res/Board/wood_4.jpg"));
}

std::shared_ptr<Image> crownstepMarbleBoardTileImage() {
	if (!APP || !APP->window) {
		return std::shared_ptr<Image>();
	}
	return APP->window->loadImage(asset::plugin(pluginInstance, "res/Board/marble_4.jpg"));
}

const crownstep::BoardState& crownstepPreviewBoardState() {
	static const crownstep::BoardState board = crownstep::makeInitialBoard();
	return board;
}

void drawCheckersPiecePreview(NVGcontext* vg, float centerX, float centerY, float cellWidth, float cellHeight, int piece, float alpha) {
	if (!vg || piece == 0) {
		return;
	}
	alpha = clamp(alpha, 0.f, 1.f);
	float radius = std::min(cellWidth, cellHeight) * 0.36f;
	int fillAlpha = int(255.f * alpha);
	int strokeAlpha = int(240.f * alpha);
	bool positivePiece = (crownstep::pieceSide(piece) == HUMAN_SIDE);

	NVGcolor coreInner = positivePiece ? nvgRGBA(237, 112, 94, fillAlpha) : nvgRGBA(78, 78, 86, fillAlpha);
	NVGcolor coreOuter = positivePiece ? nvgRGBA(152, 46, 38, fillAlpha) : nvgRGBA(12, 12, 16, fillAlpha);
	NVGcolor rimBright = positivePiece ? nvgRGBA(255, 228, 208, strokeAlpha) : nvgRGBA(210, 210, 216, strokeAlpha);
	NVGcolor rimDark = positivePiece ? nvgRGBA(82, 22, 16, int(210.f * alpha)) : nvgRGBA(6, 6, 9, int(215.f * alpha));

	nvgBeginPath(vg);
	nvgCircle(vg, centerX, centerY, radius);
	NVGpaint corePaint = nvgRadialGradient(
		vg,
		centerX - radius * 0.18f,
		centerY - radius * 0.2f,
		radius * 0.14f,
		radius * 1.06f,
		coreInner,
		coreOuter
	);
	nvgFillPaint(vg, corePaint);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, centerX, centerY, radius);
	nvgStrokeColor(vg, rimBright);
	nvgStrokeWidth(vg, 1.55f);
	nvgStroke(vg);
	nvgBeginPath(vg);
	nvgCircle(vg, centerX, centerY, radius * 0.83f);
	nvgStrokeColor(vg, rimDark);
	nvgStrokeWidth(vg, 1.05f);
	nvgStroke(vg);

	float rimOuterR = radius * 1.01f;
	float rimInnerR = radius * 0.80f;
	NVGcolor rimBandColor = positivePiece ? nvgRGBA(112, 38, 30, int(128.f * alpha)) : nvgRGBA(26, 26, 32, int(146.f * alpha));
	nvgBeginPath(vg);
	nvgCircle(vg, centerX, centerY, rimOuterR);
	nvgCircle(vg, centerX, centerY, rimInnerR);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillColor(vg, rimBandColor);
	nvgFill(vg);

	const int ridgeCount = 32;
	float step = 2.f * float(M_PI) / float(ridgeCount);
	float ridgeSpan = step * 0.56f;
	float ridgeInnerR = radius * 0.86f;
	float ridgeOuterR = radius * 1.00f;
	for (int ridge = 0; ridge < ridgeCount; ++ridge) {
		float aMid = float(ridge) * step;
		float a0 = aMid - ridgeSpan * 0.5f;
		float a1 = aMid + ridgeSpan * 0.5f;
		float c0 = std::cos(a0);
		float s0 = std::sin(a0);
		float c1 = std::cos(a1);
		float s1 = std::sin(a1);
		float x0i = centerX + c0 * ridgeInnerR;
		float y0i = centerY + s0 * ridgeInnerR;
		float x1i = centerX + c1 * ridgeInnerR;
		float y1i = centerY + s1 * ridgeInnerR;
		float x1o = centerX + c1 * ridgeOuterR;
		float y1o = centerY + s1 * ridgeOuterR;
		float x0o = centerX + c0 * ridgeOuterR;
		float y0o = centerY + s0 * ridgeOuterR;

		NVGcolor ridgeFillA = positivePiece ? nvgRGBA(248, 176, 154, int(112.f * alpha)) : nvgRGBA(172, 172, 184, int(94.f * alpha));
		NVGcolor ridgeFillB = positivePiece ? nvgRGBA(198, 104, 86, int(104.f * alpha)) : nvgRGBA(112, 112, 122, int(86.f * alpha));
		NVGcolor ridgeStroke = positivePiece ? nvgRGBA(86, 24, 18, int(110.f * alpha)) : nvgRGBA(8, 8, 12, int(116.f * alpha));

		nvgBeginPath(vg);
		nvgMoveTo(vg, x0i, y0i);
		nvgLineTo(vg, x1i, y1i);
		nvgLineTo(vg, x1o, y1o);
		nvgLineTo(vg, x0o, y0o);
		nvgClosePath(vg);
		nvgFillColor(vg, (ridge & 1) ? ridgeFillA : ridgeFillB);
		nvgFill(vg);
		nvgStrokeColor(vg, ridgeStroke);
		nvgStrokeWidth(vg, 0.34f);
		nvgStroke(vg);
	}

	nvgBeginPath(vg);
	nvgCircle(vg, centerX, centerY, rimOuterR);
	nvgStrokeColor(vg, positivePiece ? nvgRGBA(255, 212, 196, int(60.f * alpha)) : nvgRGBA(196, 196, 206, int(46.f * alpha)));
	nvgStrokeWidth(vg, 0.70f);
	nvgStroke(vg);
	nvgBeginPath(vg);
	nvgCircle(vg, centerX, centerY, rimInnerR);
	nvgStrokeColor(vg, positivePiece ? nvgRGBA(70, 18, 14, int(82.f * alpha)) : nvgRGBA(6, 6, 10, int(96.f * alpha)));
	nvgStrokeWidth(vg, 0.64f);
	nvgStroke(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, centerX - radius * 0.20f, centerY - radius * 0.24f, radius * 0.34f);
	nvgFillColor(vg, nvgRGBA(255, 255, 255, int(36.f * alpha)));
	nvgFill(vg);

	if (crownstep::pieceIsKing(piece)) {
		NVGcolor crownEdge = nvgRGBA(172, 126, 40, int(132.f * alpha));
		NVGcolor crownFillTop = nvgRGBA(255, 238, 172, int(246.f * alpha));
		NVGcolor crownFillBottom = nvgRGBA(236, 184, 70, int(244.f * alpha));
		float crownYOffset = radius * 0.07f;
		float leftX = centerX - radius * 0.56f;
		float rightX = centerX + radius * 0.56f;
		float bandTopY = centerY + radius * 0.05f + crownYOffset;
		float bandBottomY = centerY + radius * 0.24f + crownYOffset;
		float sideTipY = centerY - radius * 0.36f + crownYOffset;
		float centerTipY = centerY - radius * 0.55f + crownYOffset;
		float valleyY = centerY - radius * 0.08f + crownYOffset;
		float leftTipX = centerX - radius * 0.50f;
		float rightTipX = centerX + radius * 0.50f;
		float leftValleyX = centerX - radius * 0.18f;
		float rightValleyX = centerX + radius * 0.18f;

		nvgBeginPath(vg);
		nvgMoveTo(vg, leftX, bandTopY);
		nvgLineTo(vg, leftTipX, sideTipY);
		nvgLineTo(vg, leftValleyX, valleyY);
		nvgLineTo(vg, centerX, centerTipY);
		nvgLineTo(vg, rightValleyX, valleyY);
		nvgLineTo(vg, rightTipX, sideTipY);
		nvgLineTo(vg, rightX, bandTopY);
		nvgClosePath(vg);
		NVGpaint crownBodyPaint = nvgLinearGradient(vg, centerX, centerY - radius * 0.58f, centerX, bandBottomY, crownFillTop, crownFillBottom);
		nvgFillPaint(vg, crownBodyPaint);
		nvgFill(vg);
		nvgStrokeColor(vg, crownEdge);
		nvgStrokeWidth(vg, 0.62f);
		nvgStroke(vg);

		nvgBeginPath(vg);
		nvgRoundedRect(vg, leftX - radius * 0.03f, bandTopY, (rightX - leftX) + radius * 0.06f, bandBottomY - bandTopY, radius * 0.06f);
		nvgFillColor(vg, nvgRGBA(230, 174, 58, int(244.f * alpha)));
		nvgFill(vg);
		nvgStrokeColor(vg, crownEdge);
		nvgStrokeWidth(vg, 0.58f);
		nvgStroke(vg);

		for (int i = 0; i < 3; ++i) {
			float tipX = (i == 0) ? leftTipX : ((i == 1) ? centerX : rightTipX);
			float tipY = (i == 1) ? centerTipY : sideTipY;
			float beadR = radius * 0.07f;
			nvgBeginPath(vg);
			nvgCircle(vg, tipX, tipY, beadR);
			nvgFillColor(vg, nvgRGBA(255, 223, 120, int(246.f * alpha)));
			nvgFill(vg);
			nvgStrokeColor(vg, crownEdge);
			nvgStrokeWidth(vg, 0.55f);
			nvgStroke(vg);
		}
	}
}

constexpr const char* CHESS_ATLAS_PIECE_IDS[CHESS_ATLAS_ROWS][CHESS_ATLAS_COLS] = {
	{
		"piece-black-king",
		"piece-black-queen",
		"piece-black-bishop",
		"piece-black-knight",
		"piece-black-rook",
		"piece-black-pawn"
	},
	{
		"piece-white-king",
		"piece-white-queen",
		"piece-white-bishop",
		"piece-white-knight",
		"piece-white-rook",
		"piece-white-pawn"
	}
};

struct ChessPieceAtlasCache {
	ChessPieceAtlasCache() {
		rasterRingBandImageHandle.fill(-1);
		rasterRingShellImageHandle.fill(-1);
	}
	std::shared_ptr<Svg> svg;
	bool initialized = false;
	bool available = false;
	NVGcontext* rasterImageVg = nullptr;
	int rasterImageHandle = -1;
	NVGcontext* rasterMaskImageVg = nullptr;
	int rasterMaskImageHandle = -1;
	NVGcontext* rasterMaskGreenImageVg = nullptr;
	int rasterMaskGreenImageHandle = -1;
	NVGcontext* rasterMaskGreenDarkImageVg = nullptr;
	int rasterMaskGreenDarkImageHandle = -1;
	NVGcontext* rasterMaskSilhouetteGreenImageVg = nullptr;
	int rasterMaskSilhouetteGreenImageHandle = -1;
	NVGcontext* rasterMaskSilhouetteGreenDarkImageVg = nullptr;
	int rasterMaskSilhouetteGreenDarkImageHandle = -1;
	std::array<NVGcontext*, HIGHLIGHT_COLOR_COUNT> rasterRingBandImageVg {};
	std::array<int, HIGHLIGHT_COLOR_COUNT> rasterRingBandImageHandle {};
	std::array<NVGcontext*, HIGHLIGHT_COLOR_COUNT> rasterRingShellImageVg {};
	std::array<int, HIGHLIGHT_COLOR_COUNT> rasterRingShellImageHandle {};
	int rasterImageWidth = 0;
	int rasterImageHeight = 0;
	float rasterScale = 1.f;
	math::Rect glyphBounds[CHESS_ATLAS_ROWS][CHESS_ATLAS_COLS];
	bool hasGlyphBounds[CHESS_ATLAS_ROWS][CHESS_ATLAS_COLS] {};
	float rowMaxGlyphHeight[CHESS_ATLAS_ROWS] {};
	float maxGlyphHeight = 0.f;
};

int chessAtlasColumnForPieceType(int pieceType) {
	switch (pieceType) {
	case crownstep::CHESS_KING:
		return 0;
	case crownstep::CHESS_QUEEN:
		return 1;
	case crownstep::CHESS_BISHOP:
		return 2;
	case crownstep::CHESS_KNIGHT:
		return 3;
	case crownstep::CHESS_ROOK:
		return 4;
	case crownstep::CHESS_PAWN:
		return 5;
	default:
		return -1;
	}
}

void growGlyphBounds(math::Rect* bounds, bool* hasBounds, float minX, float minY, float maxX, float maxY) {
	if (!bounds || !hasBounds) {
		return;
	}
	if (!*hasBounds) {
		bounds->pos = Vec(minX, minY);
		bounds->size = Vec(maxX - minX, maxY - minY);
		*hasBounds = true;
		return;
	}
	float x0 = std::min(bounds->pos.x, minX);
	float y0 = std::min(bounds->pos.y, minY);
	float x1 = std::max(bounds->pos.x + bounds->size.x, maxX);
	float y1 = std::max(bounds->pos.y + bounds->size.y, maxY);
	bounds->pos = Vec(x0, y0);
	bounds->size = Vec(x1 - x0, y1 - y0);
}

bool svgShapeIdMatchesPiece(const char* shapeId, const char* pieceId) {
	if (!shapeId || !pieceId || !shapeId[0] || !pieceId[0]) {
		return false;
	}
	const std::string shapeIdText(shapeId);
	const std::string pieceIdText(pieceId);
	if (shapeIdText == pieceIdText) {
		return true;
	}
	return shapeIdText.size() > pieceIdText.size()
		&& shapeIdText.compare(0, pieceIdText.size(), pieceIdText) == 0
		&& shapeIdText[pieceIdText.size()] == '-';
}

void buildChessPieceAtlasBounds(ChessPieceAtlasCache* cache) {
	if (!cache || !cache->svg || !cache->svg->handle) {
		return;
	}

	NSVGimage* image = cache->svg->handle;
	for (NSVGshape* shape = image->shapes; shape; shape = shape->next) {
		if (!(shape->flags & NSVG_FLAGS_VISIBLE)) {
			continue;
		}
		float minX = shape->bounds[0];
		float minY = shape->bounds[1];
		float maxX = shape->bounds[2];
		float maxY = shape->bounds[3];
		if (!(maxX > minX && maxY > minY)) {
			continue;
		}
		for (int row = 0; row < CHESS_ATLAS_ROWS; ++row) {
			for (int col = 0; col < CHESS_ATLAS_COLS; ++col) {
				if (!svgShapeIdMatchesPiece(shape->id, CHESS_ATLAS_PIECE_IDS[row][col])) {
					continue;
				}
				growGlyphBounds(
					&cache->glyphBounds[row][col],
					&cache->hasGlyphBounds[row][col],
					minX, minY, maxX, maxY
				);
			}
		}
	}

	bool complete = true;
	cache->maxGlyphHeight = 0.f;
	for (int row = 0; row < CHESS_ATLAS_ROWS; ++row) {
		float rowMaxGlyphHeight = 0.f;
		for (int col = 0; col < CHESS_ATLAS_COLS; ++col) {
			if (!cache->hasGlyphBounds[row][col]) {
				complete = false;
				continue;
			}
			const float glyphHeight = cache->glyphBounds[row][col].size.y;
			rowMaxGlyphHeight = std::max(rowMaxGlyphHeight, glyphHeight);
			cache->maxGlyphHeight = std::max(cache->maxGlyphHeight, glyphHeight);
		}
		cache->rowMaxGlyphHeight[row] = rowMaxGlyphHeight;
		if (rowMaxGlyphHeight <= 0.f) {
			complete = false;
		}
	}
	if (cache->maxGlyphHeight <= 0.f) {
		complete = false;
	}
	cache->available = complete;
}

ChessPieceAtlasCache& chessPieceAtlasCache() {
	static ChessPieceAtlasCache cache;
	if (!cache.initialized) {
		cache.initialized = true;
		try {
			cache.svg = Svg::load(asset::plugin(pluginInstance, "res/chess.svg"));
		}
		catch (...) {
			cache.svg.reset();
		}
		buildChessPieceAtlasBounds(&cache);
	}
	return cache;
}

bool ensureChessPieceAtlasRasterImage(NVGcontext* vg, ChessPieceAtlasCache* cache) {
	if (!vg || !cache || !cache->available || !cache->svg || !cache->svg->handle) {
		return false;
	}
	if (cache->rasterImageHandle >= 0
		&& cache->rasterImageVg == vg
		&& cache->rasterMaskImageHandle >= 0
		&& cache->rasterMaskImageVg == vg
		&& cache->rasterMaskGreenImageHandle >= 0
		&& cache->rasterMaskGreenImageVg == vg
		&& cache->rasterMaskGreenDarkImageHandle >= 0
		&& cache->rasterMaskGreenDarkImageVg == vg
		&& cache->rasterMaskSilhouetteGreenImageHandle >= 0
		&& cache->rasterMaskSilhouetteGreenImageVg == vg
		&& cache->rasterMaskSilhouetteGreenDarkImageHandle >= 0
		&& cache->rasterMaskSilhouetteGreenDarkImageVg == vg) {
		bool haveAllRingVariants = true;
		for (int colorIndex = 0; colorIndex < HIGHLIGHT_COLOR_COUNT; ++colorIndex) {
			haveAllRingVariants = haveAllRingVariants
				&& cache->rasterRingBandImageHandle[size_t(colorIndex)] >= 0
				&& cache->rasterRingBandImageVg[size_t(colorIndex)] == vg
				&& cache->rasterRingShellImageHandle[size_t(colorIndex)] >= 0
				&& cache->rasterRingShellImageVg[size_t(colorIndex)] == vg;
		}
		if (haveAllRingVariants) {
			return true;
		}
	}

	NSVGimage* image = cache->svg->handle;
	int rasterWidth = std::max(1, int(std::ceil(image->width * CHESS_ATLAS_RASTER_SCALE)));
	int rasterHeight = std::max(1, int(std::ceil(image->height * CHESS_ATLAS_RASTER_SCALE)));
	std::vector<unsigned char> pixels(size_t(rasterWidth) * size_t(rasterHeight) * size_t(4), 0u);

	NSVGrasterizer* rasterizer = nsvgCreateRasterizer();
	if (!rasterizer) {
		return false;
	}
	nsvgRasterize(
		rasterizer,
		image,
		0.f,
		0.f,
		CHESS_ATLAS_RASTER_SCALE,
		pixels.data(),
		rasterWidth,
		rasterHeight,
		rasterWidth * 4
	);
	nsvgDeleteRasterizer(rasterizer);

	// Boost edge coverage slightly so thin SVG strokes survive Rack's runtime
	// resampling and do not appear too faint versus the source SVG.
	for (size_t i = 0, n = size_t(rasterWidth) * size_t(rasterHeight); i < n; ++i) {
		unsigned char a = pixels[i * 4 + 3];
		if (a == 0u || a == 255u) {
			continue;
		}
		float af = float(a) / 255.f;
		float boosted = std::pow(af, CHESS_ATLAS_ALPHA_GAMMA);
		pixels[i * 4 + 3] = (unsigned char) clamp(std::round(boosted * 255.f), 1.f, 255.f);
	}

	// Keep mipmaps on the base piece atlas for smoother perceived edges at panel scale.
	int imageHandle = nvgCreateImageRGBA(vg, rasterWidth, rasterHeight, CHESS_ATLAS_IMAGE_FLAGS, pixels.data());
	if (imageHandle < 0) {
		return false;
	}
	std::vector<unsigned char> maskPixels(size_t(rasterWidth) * size_t(rasterHeight) * size_t(4), 0u);
	for (size_t i = 0, n = size_t(rasterWidth) * size_t(rasterHeight); i < n; ++i) {
		unsigned char a = pixels[i * 4 + 3];
		maskPixels[i * 4 + 0] = 255u;
		maskPixels[i * 4 + 1] = 255u;
		maskPixels[i * 4 + 2] = 255u;
		maskPixels[i * 4 + 3] = a;
	}
	int maskImageHandle = nvgCreateImageRGBA(vg, rasterWidth, rasterHeight, CHESS_ATLAS_MASK_FLAGS, maskPixels.data());
	if (maskImageHandle < 0) {
		return false;
	}
	std::vector<unsigned char> greenMaskPixels(size_t(rasterWidth) * size_t(rasterHeight) * size_t(4), 0u);
	for (size_t i = 0, n = size_t(rasterWidth) * size_t(rasterHeight); i < n; ++i) {
		unsigned char a = pixels[i * 4 + 3];
		greenMaskPixels[i * 4 + 0] = 98u;
		greenMaskPixels[i * 4 + 1] = 235u;
		greenMaskPixels[i * 4 + 2] = 154u;
		greenMaskPixels[i * 4 + 3] = a;
	}
	int greenMaskImageHandle = nvgCreateImageRGBA(
		vg,
		rasterWidth,
		rasterHeight,
		CHESS_ATLAS_MASK_FLAGS,
		greenMaskPixels.data()
	);
	if (greenMaskImageHandle < 0) {
		return false;
	}
	std::vector<unsigned char> greenDarkMaskPixels(size_t(rasterWidth) * size_t(rasterHeight) * size_t(4), 0u);
	for (size_t i = 0, n = size_t(rasterWidth) * size_t(rasterHeight); i < n; ++i) {
		unsigned char a = pixels[i * 4 + 3];
		greenDarkMaskPixels[i * 4 + 0] = 40u;
		greenDarkMaskPixels[i * 4 + 1] = 168u;
		greenDarkMaskPixels[i * 4 + 2] = 104u;
		greenDarkMaskPixels[i * 4 + 3] = a;
	}
	int greenDarkMaskImageHandle = nvgCreateImageRGBA(
		vg,
		rasterWidth,
		rasterHeight,
		CHESS_ATLAS_MASK_FLAGS,
		greenDarkMaskPixels.data()
	);
	if (greenDarkMaskImageHandle < 0) {
		return false;
	}
	// Build a silhouette alpha mask where internal transparent "holes" are filled,
	// so ring contours stay strictly outside the piece outline.
	const size_t pixelCount = size_t(rasterWidth) * size_t(rasterHeight);
	std::vector<uint8_t> solid(pixelCount, 0u);
	std::vector<uint8_t> outside(pixelCount, 0u);
	std::vector<int> queue;
	queue.reserve(pixelCount / 8u);
	for (size_t i = 0; i < pixelCount; ++i) {
		solid[i] = (pixels[i * 4 + 3] > CHESS_MASK_SOLID_ALPHA_THRESHOLD) ? 1u : 0u;
	}
	auto tryEnqueueOutside = [&](int x, int y) {
		if (x < 0 || y < 0 || x >= rasterWidth || y >= rasterHeight) {
			return;
		}
		size_t idx = size_t(y) * size_t(rasterWidth) + size_t(x);
		if (solid[idx] || outside[idx]) {
			return;
		}
		outside[idx] = 1u;
		queue.push_back(int(idx));
	};
	for (int x = 0; x < rasterWidth; ++x) {
		tryEnqueueOutside(x, 0);
		tryEnqueueOutside(x, rasterHeight - 1);
	}
	for (int y = 0; y < rasterHeight; ++y) {
		tryEnqueueOutside(0, y);
		tryEnqueueOutside(rasterWidth - 1, y);
	}
	for (size_t head = 0; head < queue.size(); ++head) {
		int idx = queue[head];
		int x = idx % rasterWidth;
		int y = idx / rasterWidth;
		tryEnqueueOutside(x - 1, y);
		tryEnqueueOutside(x + 1, y);
		tryEnqueueOutside(x, y - 1);
		tryEnqueueOutside(x, y + 1);
	}
	std::vector<unsigned char> silhouetteGreenMaskPixels(pixelCount * size_t(4), 0u);
	std::vector<unsigned char> silhouetteGreenDarkMaskPixels(pixelCount * size_t(4), 0u);
	for (size_t i = 0; i < pixelCount; ++i) {
		unsigned char srcA = pixels[i * 4 + 3];
		unsigned char silhouetteA = 0u;
		if (solid[i]) {
			silhouetteA = srcA;
		}
		else if (!outside[i]) {
			silhouetteA = 255u;
		}
		silhouetteGreenMaskPixels[i * 4 + 0] = 98u;
		silhouetteGreenMaskPixels[i * 4 + 1] = 235u;
		silhouetteGreenMaskPixels[i * 4 + 2] = 154u;
		silhouetteGreenMaskPixels[i * 4 + 3] = silhouetteA;
		silhouetteGreenDarkMaskPixels[i * 4 + 0] = 40u;
		silhouetteGreenDarkMaskPixels[i * 4 + 1] = 168u;
		silhouetteGreenDarkMaskPixels[i * 4 + 2] = 104u;
		silhouetteGreenDarkMaskPixels[i * 4 + 3] = silhouetteA;
	}
	int silhouetteGreenMaskImageHandle = nvgCreateImageRGBA(
		vg,
		rasterWidth,
		rasterHeight,
		CHESS_ATLAS_MASK_FLAGS,
		silhouetteGreenMaskPixels.data()
	);
	if (silhouetteGreenMaskImageHandle < 0) {
		return false;
	}
	int silhouetteGreenDarkMaskImageHandle = nvgCreateImageRGBA(
		vg,
		rasterWidth,
		rasterHeight,
		CHESS_ATLAS_MASK_FLAGS,
		silhouetteGreenDarkMaskPixels.data()
	);
	if (silhouetteGreenDarkMaskImageHandle < 0) {
		return false;
	}
	// Build distance-based external contour bands (offset from silhouette).
	// This approximates a uniform normal-distance offset around the piece.
	std::vector<float> outsideDist(pixelCount, 1e9f);
	for (size_t i = 0; i < pixelCount; ++i) {
		if (!outside[i]) {
			outsideDist[i] = 0.f;
		}
	}
	const float kDiag = 1.41421356f;
	for (int y = 0; y < rasterHeight; ++y) {
		for (int x = 0; x < rasterWidth; ++x) {
			size_t idx = size_t(y) * size_t(rasterWidth) + size_t(x);
			if (!outside[idx]) {
				continue;
			}
			float d = outsideDist[idx];
			if (x > 0) {
				d = std::min(d, outsideDist[idx - 1] + 1.f);
			}
			if (y > 0) {
				d = std::min(d, outsideDist[idx - size_t(rasterWidth)] + 1.f);
			}
			if (x > 0 && y > 0) {
				d = std::min(d, outsideDist[idx - size_t(rasterWidth) - 1] + kDiag);
			}
			if (x + 1 < rasterWidth && y > 0) {
				d = std::min(d, outsideDist[idx - size_t(rasterWidth) + 1] + kDiag);
			}
			outsideDist[idx] = d;
		}
	}
	for (int y = rasterHeight - 1; y >= 0; --y) {
		for (int x = rasterWidth - 1; x >= 0; --x) {
			size_t idx = size_t(y) * size_t(rasterWidth) + size_t(x);
			if (!outside[idx]) {
				continue;
			}
			float d = outsideDist[idx];
			if (x + 1 < rasterWidth) {
				d = std::min(d, outsideDist[idx + 1] + 1.f);
			}
			if (y + 1 < rasterHeight) {
				d = std::min(d, outsideDist[idx + size_t(rasterWidth)] + 1.f);
			}
			if (x + 1 < rasterWidth && y + 1 < rasterHeight) {
				d = std::min(d, outsideDist[idx + size_t(rasterWidth) + 1] + kDiag);
			}
			if (x > 0 && y + 1 < rasterHeight) {
				d = std::min(d, outsideDist[idx + size_t(rasterWidth) - 1] + kDiag);
			}
			outsideDist[idx] = d;
		}
	}
	auto smoothstep01 = [](float t) {
		t = clamp(t, 0.f, 1.f);
		return t * t * (3.f - 2.f * t);
	};
	// Distance windows in raster pixels. Bias toward a slightly broader bright band
	// so the contour reads bold without needing a washed-out high alpha.
	const float bandInPx = 2.0f;
	const float bandOutPx = 14.8f;
	const float shellInPx = 13.0f;
	const float shellOutPx = 22.6f;
	std::array<std::vector<unsigned char>, HIGHLIGHT_COLOR_COUNT> ringBandPixels;
	std::array<std::vector<unsigned char>, HIGHLIGHT_COLOR_COUNT> ringShellPixels;
	for (int colorIndex = 0; colorIndex < HIGHLIGHT_COLOR_COUNT; ++colorIndex) {
		ringBandPixels[size_t(colorIndex)].assign(pixelCount * size_t(4), 0u);
		ringShellPixels[size_t(colorIndex)].assign(pixelCount * size_t(4), 0u);
	}
	for (size_t i = 0; i < pixelCount; ++i) {
		if (!outside[i]) {
			continue;
		}
		float d = outsideDist[i];
		float bandEnter = smoothstep01((d - bandInPx) / 1.6f);
		float bandExit = smoothstep01((bandOutPx - d) / 1.6f);
		float bandA = clamp(bandEnter * bandExit, 0.f, 1.f);
		float shellEnter = smoothstep01((d - shellInPx) / 1.8f);
		float shellExit = smoothstep01((shellOutPx - d) / 1.8f);
		float shellA = clamp(shellEnter * shellExit, 0.f, 1.f);
		for (int colorIndex = 0; colorIndex < HIGHLIGHT_COLOR_COUNT; ++colorIndex) {
			const HighlightPalette palette = HIGHLIGHT_PALETTES[size_t(colorIndex)];
			ringBandPixels[size_t(colorIndex)][i * 4 + 0] = palette.bandR;
			ringBandPixels[size_t(colorIndex)][i * 4 + 1] = palette.bandG;
			ringBandPixels[size_t(colorIndex)][i * 4 + 2] = palette.bandB;
			ringBandPixels[size_t(colorIndex)][i * 4 + 3] = (unsigned char) clamp(248.f * bandA, 0.f, 255.f);
			ringShellPixels[size_t(colorIndex)][i * 4 + 0] = palette.shellR;
			ringShellPixels[size_t(colorIndex)][i * 4 + 1] = palette.shellG;
			ringShellPixels[size_t(colorIndex)][i * 4 + 2] = palette.shellB;
			ringShellPixels[size_t(colorIndex)][i * 4 + 3] = (unsigned char) clamp(192.f * shellA, 0.f, 255.f);
		}
	}
	for (int colorIndex = 0; colorIndex < HIGHLIGHT_COLOR_COUNT; ++colorIndex) {
		int ringBandImageHandle = nvgCreateImageRGBA(
			vg,
			rasterWidth,
			rasterHeight,
			CHESS_ATLAS_MASK_FLAGS,
			ringBandPixels[size_t(colorIndex)].data()
		);
		if (ringBandImageHandle < 0) {
			return false;
		}
		int ringShellImageHandle = nvgCreateImageRGBA(
			vg,
			rasterWidth,
			rasterHeight,
			CHESS_ATLAS_MASK_FLAGS,
			ringShellPixels[size_t(colorIndex)].data()
		);
		if (ringShellImageHandle < 0) {
			return false;
		}
		cache->rasterRingBandImageVg[size_t(colorIndex)] = vg;
		cache->rasterRingBandImageHandle[size_t(colorIndex)] = ringBandImageHandle;
		cache->rasterRingShellImageVg[size_t(colorIndex)] = vg;
		cache->rasterRingShellImageHandle[size_t(colorIndex)] = ringShellImageHandle;
	}
	cache->rasterImageVg = vg;
	cache->rasterImageHandle = imageHandle;
	cache->rasterMaskImageVg = vg;
	cache->rasterMaskImageHandle = maskImageHandle;
	cache->rasterMaskGreenImageVg = vg;
	cache->rasterMaskGreenImageHandle = greenMaskImageHandle;
	cache->rasterMaskGreenDarkImageVg = vg;
	cache->rasterMaskGreenDarkImageHandle = greenDarkMaskImageHandle;
	cache->rasterMaskSilhouetteGreenImageVg = vg;
	cache->rasterMaskSilhouetteGreenImageHandle = silhouetteGreenMaskImageHandle;
	cache->rasterMaskSilhouetteGreenDarkImageVg = vg;
	cache->rasterMaskSilhouetteGreenDarkImageHandle = silhouetteGreenDarkMaskImageHandle;
	cache->rasterImageWidth = rasterWidth;
	cache->rasterImageHeight = rasterHeight;
	cache->rasterScale = CHESS_ATLAS_RASTER_SCALE;
	return true;
}

struct ChessAtlasDrawSpec {
	math::Rect src;
	float drawWidth = 0.f;
	float drawHeight = 0.f;
	float x = 0.f;
	float y = 0.f;
	float atlasDrawWidth = 0.f;
	float atlasDrawHeight = 0.f;
	float patternX = 0.f;
	float patternY = 0.f;
};

bool buildChessAtlasDrawSpec(
	const ChessPieceAtlasCache& cache,
	float centerX,
	float centerY,
	float cellWidth,
	float cellHeight,
	int piece,
	ChessAtlasDrawSpec* spec
) {
	if (!spec || piece == 0) {
		return false;
	}
	int pieceType = std::abs(piece);
	int col = chessAtlasColumnForPieceType(pieceType);
	if (col < 0) {
		return false;
	}
	int row = (piece > 0) ? 1 : 0;
	if (!cache.hasGlyphBounds[row][col]) {
		return false;
	}

	const math::Rect src = cache.glyphBounds[row][col];
	if (src.size.x <= 0.f || src.size.y <= 0.f) {
		return false;
	}
	float scaleRefHeight = 0.f;
	if (CHESS_ATLAS_SCALE_PER_COLOR) {
		scaleRefHeight = cache.rowMaxGlyphHeight[row];
	}
	else {
		scaleRefHeight = cache.maxGlyphHeight;
	}
	if (scaleRefHeight <= 0.f || !cache.svg || !cache.svg->handle) {
		return false;
	}

	const float targetH = std::min(cellWidth, cellHeight) * 0.90f;
	const float scale = targetH / scaleRefHeight;
	const float drawWidth = src.size.x * scale;
	const float drawHeight = src.size.y * scale;
	const float yOffset = std::min(cellWidth, cellHeight) * 0.03f;
	const float baselineY = centerY + targetH * 0.5f + yOffset;
	const float x = centerX - drawWidth * 0.5f;
	const float y = baselineY - drawHeight;
	const float atlasDrawWidth = cache.svg->handle->width * scale;
	const float atlasDrawHeight = cache.svg->handle->height * scale;
	const float patternX = x - src.pos.x * scale;
	const float patternY = y - src.pos.y * scale;

	spec->src = src;
	spec->drawWidth = drawWidth;
	spec->drawHeight = drawHeight;
	spec->x = x;
	spec->y = y;
	spec->atlasDrawWidth = atlasDrawWidth;
	spec->atlasDrawHeight = atlasDrawHeight;
	spec->patternX = patternX;
	spec->patternY = patternY;
	return true;
}

bool drawChessAtlasPiece(
	NVGcontext* vg,
	float centerX,
	float centerY,
	float cellWidth,
	float cellHeight,
	int piece,
	float alpha
) {
	if (!vg || piece == 0) {
		return false;
	}
	ChessPieceAtlasCache& cache = chessPieceAtlasCache();
	if (!cache.available || !cache.svg || !ensureChessPieceAtlasRasterImage(vg, &cache)) {
		return false;
	}
	ChessAtlasDrawSpec spec;
	if (!buildChessAtlasDrawSpec(cache, centerX, centerY, cellWidth, cellHeight, piece, &spec)) {
		return false;
	}

	nvgSave(vg);
	nvgBeginPath(vg);
	nvgRect(vg, spec.x, spec.y, spec.drawWidth, spec.drawHeight);
	NVGpaint imagePaint = nvgImagePattern(
		vg,
		spec.patternX,
		spec.patternY,
		spec.atlasDrawWidth,
		spec.atlasDrawHeight,
		0.f,
		cache.rasterImageHandle,
		clamp(alpha, 0.f, 1.f)
	);
	nvgFillPaint(vg, imagePaint);
	nvgFill(vg);
	nvgRestore(vg);
	return true;
}

bool drawChessAtlasPieceRingContour(
	NVGcontext* vg,
	float centerX,
	float centerY,
	float cellWidth,
	float cellHeight,
	int piece,
	int highlightMode,
	float pulse
) {
	if (!vg || piece == 0) {
		return false;
	}
	ChessPieceAtlasCache& cache = chessPieceAtlasCache();
	if (!cache.available
		|| !cache.svg
		|| !ensureChessPieceAtlasRasterImage(vg, &cache)
		|| highlightMode == Crownstep::HIGHLIGHT_OFF) {
		return false;
	}
	const int paletteIndex = highlightPaletteIndexForMode(highlightMode);
	if (cache.rasterRingBandImageHandle[size_t(paletteIndex)] < 0
		|| cache.rasterRingShellImageHandle[size_t(paletteIndex)] < 0) {
		return false;
	}

	ChessAtlasDrawSpec spec;
	if (!buildChessAtlasDrawSpec(cache, centerX, centerY, cellWidth, cellHeight, piece, &spec)) {
		return false;
	}

	// Draw precomputed distance-field contour masks with fixed atlas mapping.
	// Only the revealed outer margin breathes, which avoids shape-specific edge
	// artifacts from rescaling the contour texture itself.
	float atlasScale = spec.atlasDrawWidth / std::max(1.f, cache.svg->handle->width);
	float pxToScreen = atlasScale / std::max(0.001f, CHESS_ATLAS_RASTER_SCALE);
	// Must cover the largest precomputed raster-distance shell extents.
	float shellMargin = 25.5f * pxToScreen;
	float bandMargin = 16.4f * pxToScreen;
	float pulse01 = clamp(pulse, 0.f, 1.f);
	// Keep pulse expansion conservative; large expansion can leak neighboring
	// atlas content near cell edges and show as corner glow artifacts.
	float shellPulseMargin = shellMargin + pxToScreen * (0.35f + 1.45f * pulse01);
	float bandPulseMargin = bandMargin + pxToScreen * (0.25f + 1.05f * pulse01);
	float shellAlpha = 0.24f + 0.28f * pulse01;
	float bandAlpha = 0.78f + 0.18f * pulse01;

	auto drawRingMaskLayer = [&](int imageHandle, float margin, float alpha) {
		float x = spec.x - margin;
		float y = spec.y - margin;
		float w = spec.drawWidth + 2.f * margin;
		float h = spec.drawHeight + 2.f * margin;

		nvgBeginPath(vg);
		nvgRect(vg, x, y, w, h);
		NVGpaint ringPaint = nvgImagePattern(
			vg,
			spec.patternX,
			spec.patternY,
			spec.atlasDrawWidth,
			spec.atlasDrawHeight,
			0.f,
			imageHandle,
			clamp(alpha, 0.f, 1.f)
		);
		nvgFillPaint(vg, ringPaint);
		nvgFill(vg);
	};

	nvgSave(vg);
	drawRingMaskLayer(cache.rasterRingShellImageHandle[size_t(paletteIndex)], shellPulseMargin, shellAlpha);
	drawRingMaskLayer(cache.rasterRingBandImageHandle[size_t(paletteIndex)], bandPulseMargin, bandAlpha);
	nvgRestore(vg);
	return true;
}

} // namespace

struct CrownstepBoardWidget final : Widget {
	Crownstep* module = nullptr;

	explicit CrownstepBoardWidget(Crownstep* crownstepModule) {
		module = crownstepModule;
	}

	int indexFromLocalPos(Vec pos) const {
		if (!box.size.x || !box.size.y) {
			return -1;
		}
		if (pos.x < 0.f || pos.y < 0.f || pos.x >= box.size.x || pos.y >= box.size.y) {
			return -1;
		}
		float cellWidth = box.size.x / 8.f;
		float cellHeight = box.size.y / 8.f;
		int col = clamp(int(pos.x / cellWidth), 0, 7);
		int row = clamp(int(pos.y / cellHeight), 0, 7);
		bool rotateBoardForHumanPerspective =
			module
			&& module->humanSide() == AI_SIDE
			&& (module->gameMode == Crownstep::GAME_MODE_CHECKERS || module->isChessMode());
		if (rotateBoardForHumanPerspective) {
			row = 7 - row;
			col = 7 - col;
		}
		return module ? module->boardCoordToIndex(row, col) : crownstep::coordToIndex(row, col);
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			Widget::onButton(e);
			return;
		}
		int index = indexFromLocalPos(e.pos);
		if (index >= 0) {
			module->onBoardSquarePressed(index);
			e.consume(this);
			return;
		}
		if (!module->isChessMode() && !module->isOthelloMode() && module->selectedSquare >= 0) {
			module->selectedSquare = -1;
			module->refreshLegalMoves();
			e.consume(this);
			return;
		}
		Widget::onButton(e);
	}

	void onHover(const event::Hover& e) override {
		if (module) {
			module->setHoveredSquare(indexFromLocalPos(e.pos));
		}
		Widget::onHover(e);
	}

	void onLeave(const event::Leave& e) override {
		if (module) {
			module->setHoveredSquare(-1);
		}
		Widget::onLeave(e);
	}

	void draw(const DrawArgs& args) override {
		nvgSave(args.vg);

		float cellWidth = box.size.x / 8.f;
		float cellHeight = box.size.y / 8.f;
		const int effectiveGameMode = module ? module->gameMode : Crownstep::GAME_MODE_CHECKERS;
		const int effectiveBoardTextureMode = module ? module->boardTextureMode : Crownstep::BOARD_TEXTURE_MARBLE;
		const crownstep::BoardState& previewBoard = crownstepPreviewBoardState();
		const bool rotateBoardForHumanPerspective =
			module
			&& module->humanSide() == AI_SIDE
			&& (effectiveGameMode == Crownstep::GAME_MODE_CHECKERS || module->isChessMode());
		auto viewRowColFromBoardIndex = [&](int index, int* row, int* col) {
			if (module) {
				if (!module->boardIndexToCoord(index, row, col)) {
					return false;
				}
			}
			else if (!crownstep::indexToCoord(index, row, col)) {
				return false;
			}
			if (rotateBoardForHumanPerspective) {
				if (row) {
					*row = 7 - *row;
				}
				if (col) {
					*col = 7 - *col;
				}
		}
			return true;
		};
		bool othelloBoard = module && module->isOthelloMode();
		const bool woodTexture = effectiveBoardTextureMode == Crownstep::BOARD_TEXTURE_WOOD;
		const bool marbleTexture = effectiveBoardTextureMode == Crownstep::BOARD_TEXTURE_MARBLE;
		const bool redBlackTexture = effectiveBoardTextureMode == Crownstep::BOARD_TEXTURE_RED_BLACK;
		const bool whiteBlackTexture = effectiveBoardTextureMode == Crownstep::BOARD_TEXTURE_WHITE_BLACK;
		const std::shared_ptr<Image>& woodBoardTileImage = (!othelloBoard && woodTexture) ? crownstepWoodBoardTileImage() : std::shared_ptr<Image>();
		const std::shared_ptr<Image>& marbleBoardTileImage = (!othelloBoard && marbleTexture) ? crownstepMarbleBoardTileImage() : std::shared_ptr<Image>();
		const bool hasWoodBoardTileImage = woodBoardTileImage && woodBoardTileImage->handle >= 0;
		const bool hasMarbleBoardTileImage = marbleBoardTileImage && marbleBoardTileImage->handle >= 0;
		auto paintBoardBitmap = [&](int imageHandle) {
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
			NVGpaint boardPaint = nvgImagePattern(
				args.vg,
				0.f,
				0.f,
				box.size.x * 0.5f,
				box.size.y * 0.5f,
				0.f,
				imageHandle,
				1.f
			);
			nvgFillPaint(args.vg, boardPaint);
			nvgFill(args.vg);
		};
		if (hasWoodBoardTileImage) {
			paintBoardBitmap(woodBoardTileImage->handle);
		}
		else if (hasMarbleBoardTileImage) {
			paintBoardBitmap(marbleBoardTileImage->handle);
		}
		for (int row = 0; row < 8; ++row) {
			for (int col = 0; col < 8; ++col) {
				bool dark = ((row + col) & 1) == 1;
				bool fabricTexture = effectiveBoardTextureMode == Crownstep::BOARD_TEXTURE_FABRIC;
				float x = col * cellWidth;
				float y = row * cellHeight;
				float seed = float((row * 29 + col * 17) % 97) * 0.17f;
				if (othelloBoard) {
					// Othello board: green felt/table look with subtle per-cell variation.
					float tint = 0.5f + 0.5f * std::sin(seed * 1.7f + float((row + col) & 1) * 0.9f);
					NVGcolor topColor = nvgRGBA(
						int(18.f + 8.f * tint),
						int(104.f + 16.f * tint),
						int(56.f + 10.f * tint),
						255
					);
					NVGcolor bottomColor = nvgRGBA(
						int(10.f + 6.f * tint),
						int(72.f + 11.f * tint),
						int(40.f + 8.f * tint),
						255
					);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
					nvgFillPaint(args.vg, basePaint);
					nvgFill(args.vg);
				}
				else if (marbleTexture && !hasMarbleBoardTileImage) {
					NVGcolor topColor = dark ? nvgRGB(72, 74, 82) : nvgRGB(208, 212, 222);
					NVGcolor bottomColor = dark ? nvgRGB(42, 44, 52) : nvgRGB(166, 172, 184);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
					nvgFillPaint(args.vg, basePaint);
					nvgFill(args.vg);

					NVGcolor cloudA = dark ? nvgRGBA(116, 122, 140, 30) : nvgRGBA(255, 255, 255, 44);
					NVGcolor cloudB = dark ? nvgRGBA(30, 34, 46, 22) : nvgRGBA(144, 152, 168, 28);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint cloudPaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y + cellHeight, cloudA, cloudB);
					nvgFillPaint(args.vg, cloudPaint);
					nvgFill(args.vg);

					for (int vein = 0; vein < 3; ++vein) {
						float t = (vein + 1.f) / 4.f;
						float y0 = y + t * cellHeight + std::sin(seed * 0.9f + float(vein) * 1.8f) * 0.9f;
						float y1 = y0 + std::cos(seed * 0.6f + float(vein) * 1.5f) * 1.1f;
						NVGcolor veinColor = dark ? nvgRGBA(178, 186, 206, 44) : nvgRGBA(116, 124, 144, 54);
						nvgBeginPath(args.vg);
						nvgMoveTo(args.vg, x + 0.8f, y0);
						nvgBezierTo(args.vg, x + cellWidth * 0.30f, y0 - 1.1f, x + cellWidth * 0.65f, y1 + 1.1f, x + cellWidth - 0.8f, y1);
						nvgStrokeColor(args.vg, veinColor);
						nvgStrokeWidth(args.vg, 0.62f + 0.12f * float(vein & 1));
						nvgStroke(args.vg);
					}
				}
				else if (marbleTexture) {
					// Tiled marble bitmap already painted above.
				}
				else if (redBlackTexture) {
					NVGcolor topColor = dark ? nvgRGB(32, 32, 36) : nvgRGB(136, 26, 32);
					NVGcolor bottomColor = dark ? nvgRGB(14, 14, 18) : nvgRGB(88, 12, 18);
					NVGcolor sheenA = dark ? nvgRGBA(255, 255, 255, 14) : nvgRGBA(255, 212, 214, 18);
					NVGcolor sheenB = dark ? nvgRGBA(0, 0, 0, 22) : nvgRGBA(38, 4, 6, 18);

					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
					nvgFillPaint(args.vg, basePaint);
					nvgFill(args.vg);

					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint sheenPaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y + cellHeight, sheenA, sheenB);
					nvgFillPaint(args.vg, sheenPaint);
					nvgFill(args.vg);
				}
				else if (whiteBlackTexture) {
					NVGcolor topColor = dark ? nvgRGB(36, 36, 40) : nvgRGB(248, 248, 252);
					NVGcolor bottomColor = dark ? nvgRGB(12, 12, 16) : nvgRGB(226, 228, 234);
					NVGcolor sheenA = dark ? nvgRGBA(255, 255, 255, 12) : nvgRGBA(255, 255, 255, 20);
					NVGcolor sheenB = dark ? nvgRGBA(0, 0, 0, 20) : nvgRGBA(70, 74, 84, 12);

					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
					nvgFillPaint(args.vg, basePaint);
					nvgFill(args.vg);

					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint sheenPaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y + cellHeight, sheenA, sheenB);
					nvgFillPaint(args.vg, sheenPaint);
					nvgFill(args.vg);
				}
				else if (fabricTexture) {
					NVGcolor topColor = dark ? nvgRGB(42, 124, 62) : nvgRGB(236, 243, 234);
					NVGcolor bottomColor = dark ? nvgRGB(16, 74, 34) : nvgRGB(208, 220, 204);
					NVGcolor washA = dark ? nvgRGBA(255, 255, 255, 12) : nvgRGBA(255, 255, 255, 16);
					NVGcolor washB = dark ? nvgRGBA(8, 28, 12, 18) : nvgRGBA(72, 94, 70, 14);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
					nvgFillPaint(args.vg, basePaint);
					nvgFill(args.vg);

					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint washPaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y + cellHeight, washA, washB);
					nvgFillPaint(args.vg, washPaint);
					nvgFill(args.vg);
				}
				else if (woodTexture) {
					if (hasWoodBoardTileImage) {
						NVGcolor sheenLeft = dark ? nvgRGBA(255, 226, 176, 18) : nvgRGBA(255, 244, 214, 30);
						NVGcolor sheenRight = dark ? nvgRGBA(28, 16, 8, 18) : nvgRGBA(92, 62, 36, 20);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, x, y, cellWidth, cellHeight);
						NVGpaint sheenPaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y, sheenLeft, sheenRight);
						nvgFillPaint(args.vg, sheenPaint);
						nvgFill(args.vg);
					}
					else {
						NVGcolor topColor = dark ? nvgRGB(112, 78, 50) : nvgRGB(224, 198, 160);
						NVGcolor bottomColor = dark ? nvgRGB(72, 46, 30) : nvgRGB(186, 147, 102);

						nvgBeginPath(args.vg);
						nvgRect(args.vg, x, y, cellWidth, cellHeight);
						NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
						nvgFillPaint(args.vg, basePaint);
						nvgFill(args.vg);

						NVGcolor sheenLeft = dark ? nvgRGBA(255, 216, 156, 20) : nvgRGBA(255, 236, 198, 34);
						NVGcolor sheenRight = dark ? nvgRGBA(40, 20, 8, 22) : nvgRGBA(76, 48, 24, 24);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, x, y, cellWidth, cellHeight);
						NVGpaint sheenPaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y, sheenLeft, sheenRight);
						nvgFillPaint(args.vg, sheenPaint);
						nvgFill(args.vg);

						for (int grain = 0; grain < 4; ++grain) {
							float t = (grain + 1) / 5.f;
							float grainY = y + t * cellHeight + std::sin(seed + t * 6.28318f) * 0.65f;
							float thickness = 0.5f + 0.15f * float(grain & 1);
							int alpha = dark ? 28 : 24;

							nvgBeginPath(args.vg);
							nvgRect(args.vg, x + 0.65f, grainY, cellWidth - 1.3f, thickness);
							nvgFillColor(args.vg, nvgRGBA(255, 224, 170, alpha));
							nvgFill(args.vg);
						}
					}
				}
				else {
					NVGcolor topColor = dark ? nvgRGB(112, 78, 50) : nvgRGB(224, 198, 160);
					NVGcolor bottomColor = dark ? nvgRGB(72, 46, 30) : nvgRGB(186, 147, 102);

					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
					nvgFillPaint(args.vg, basePaint);
					nvgFill(args.vg);

					NVGcolor sheenLeft = dark ? nvgRGBA(255, 216, 156, 20) : nvgRGBA(255, 236, 198, 34);
					NVGcolor sheenRight = dark ? nvgRGBA(40, 20, 8, 22) : nvgRGBA(76, 48, 24, 24);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint sheenPaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y, sheenLeft, sheenRight);
					nvgFillPaint(args.vg, sheenPaint);
					nvgFill(args.vg);

					for (int grain = 0; grain < 4; ++grain) {
						float t = (grain + 1) / 5.f;
						float grainY = y + t * cellHeight + std::sin(seed + t * 6.28318f) * 0.65f;
						float thickness = 0.5f + 0.15f * float(grain & 1);
						int alpha = dark ? 28 : 24;

						nvgBeginPath(args.vg);
						nvgRect(args.vg, x + 0.65f, grainY, cellWidth - 1.3f, thickness);
						nvgFillColor(args.vg, nvgRGBA(255, 224, 170, alpha));
						nvgFill(args.vg);
					}
				}
			}
		}
		if (othelloBoard) {
			NVGcolor gridColor = nvgRGBA(8, 42, 24, 214);
			nvgBeginPath(args.vg);
			for (int i = 0; i <= 8; ++i) {
				float x = float(i) * cellWidth;
				nvgMoveTo(args.vg, x, 0.f);
				nvgLineTo(args.vg, x, box.size.y);
			}
			for (int i = 0; i <= 8; ++i) {
				float y = float(i) * cellHeight;
				nvgMoveTo(args.vg, 0.f, y);
				nvgLineTo(args.vg, box.size.x, y);
			}
			nvgStrokeColor(args.vg, gridColor);
			nvgStrokeWidth(args.vg, 1.1f);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0.7f, 0.7f, box.size.x - 1.4f, box.size.y - 1.4f);
			nvgStrokeColor(args.vg, nvgRGBA(4, 24, 14, 228));
			nvgStrokeWidth(args.vg, 1.6f);
			nvgStroke(args.vg);
		}

		if (!module) {
			for (int i = 0; i < crownstep::BOARD_SIZE; ++i) {
				int piece = previewBoard[size_t(i)];
				if (piece == 0) {
					continue;
				}
				int row = 0;
				int col = 0;
				if (!viewRowColFromBoardIndex(i, &row, &col)) {
					continue;
				}
				float centerX = (col + 0.5f) * cellWidth;
				float centerY = (row + 0.5f) * cellHeight;
				drawCheckersPiecePreview(args.vg, centerX, centerY, cellWidth, cellHeight, piece, 1.f);
			}
			nvgRestore(args.vg);
			return;
		}

					if (module) {
						auto drawCellPitchOverlay = [&]() {
							if (!module->showCellPitchOverlay) {
								return;
							}
							float valueFontSize = std::max(5.2f, std::min(cellWidth, cellHeight) * 0.19f);
							int cellCount = module->boardCellCount();
							for (int boardIndex = 0; boardIndex < cellCount; ++boardIndex) {
								int row = 0;
								int col = 0;
								if (!viewRowColFromBoardIndex(boardIndex, &row, &col)) {
									continue;
								}
								float valueVolts = module->pitchPreviewForBoardIndex(boardIndex);
								char valueText[24];
								std::snprintf(valueText, sizeof(valueText), "%.2f", valueVolts);
								float textX = col * cellWidth + 1.6f;
								float textY = row * cellHeight + 1.8f;
								bool darkSquare = othelloBoard || (((row + col) & 1) == 1);
								NVGcolor textColor = darkSquare ? nvgRGBA(242, 246, 252, 224) : nvgRGBA(20, 24, 30, 220);
								NVGcolor shadowColor = darkSquare ? nvgRGBA(8, 10, 14, 186) : nvgRGBA(252, 252, 255, 158);
								nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
								nvgFontSize(args.vg, valueFontSize);
								nvgFillColor(args.vg, shadowColor);
								nvgText(args.vg, textX + 0.55f, textY + 0.55f, valueText, nullptr);
								nvgFillColor(args.vg, textColor);
								nvgText(args.vg, textX, textY, valueText, nullptr);
							}
						};
						// Keep animation phase numerically stable during very long sessions.
						float animTime = float(std::fmod(double(module->transportTimeSeconds), 4096.0));
					const bool highlightVisible = module->highlightMode != Crownstep::HIGHLIGHT_OFF;
					// Temporary UX tweak: hide AI potential-move hint dots/rings.
					const bool renderOpponentMoveHints = false;
					if (highlightVisible && !module->gameOver && module->selectedSquare >= 0) {
					int row = 0;
					int col = 0;
					if (viewRowColFromBoardIndex(module->selectedSquare, &row, &col)) {
						float pulse = 0.5f + 0.5f * std::sin(animTime * 4.6f + 0.8f);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth - 1.f, row * cellHeight - 1.f, cellWidth + 2.f, cellHeight + 2.f);
						nvgFillColor(args.vg, highlightGlowColor(module->highlightMode, int(24.f + 48.f * pulse)));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth, row * cellHeight, cellWidth, cellHeight);
						nvgStrokeColor(args.vg, highlightBandColor(module->highlightMode, int(188.f + 54.f * pulse)));
						nvgStrokeWidth(args.vg, 2.15f);
						nvgStroke(args.vg);
					}
				}
				if (highlightVisible && !module->gameOver) {
					for (int destinationIndex : module->highlightedDestinations) {
						int row = 0;
						int col = 0;
						if (!viewRowColFromBoardIndex(destinationIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.43f;
						float breath = 0.5f + 0.5f * std::sin(animTime * 4.4f + phase);
						float glowRadius = std::min(cellWidth, cellHeight) * (0.17f + 0.06f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, glowRadius);
						nvgFillColor(args.vg, highlightGlowColor(module->highlightMode, int(44.f + 50.f * breath)));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.105f);
						nvgFillColor(args.vg, highlightBandColor(module->highlightMode, 255));
						nvgFill(args.vg);
					}
						if (renderOpponentMoveHints) for (int destinationIndex : module->opponentHighlightedDestinations) {
							bool overlapsHuman = false;
						for (int humanDestination : module->highlightedDestinations) {
							if (humanDestination == destinationIndex) {
								overlapsHuman = true;
								break;
							}
						}
						if (overlapsHuman) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!viewRowColFromBoardIndex(destinationIndex, &row, &col)) {
							continue;
						}
						if (module->opponentHintsPreviewActive && module->board[size_t(destinationIndex)] != 0) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.39f + 1.7f;
						float breath = 0.5f + 0.5f * std::sin(animTime * 4.1f + phase);
						float glowRadius = std::min(cellWidth, cellHeight) * (0.16f + 0.055f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, glowRadius);
						nvgFillColor(args.vg, highlightShellColor(module->highlightMode, int(36.f + 48.f * breath)));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.092f);
						nvgFillColor(args.vg, highlightBandColor(module->highlightMode, 255));
						nvgFill(args.vg);
					}
				}
				if (!module->gameOver && module->lastMove.originIndex >= 0) {
					for (int highlightIndex : {module->lastMove.originIndex, module->lastMove.destinationIndex}) {
						int row = 0;
						int col = 0;
						if (!viewRowColFromBoardIndex(highlightIndex, &row, &col)) {
							continue;
						}
						float phase = float(highlightIndex) * 0.34f;
						float pulse = 0.5f + 0.5f * std::sin(animTime * 3.8f + phase);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth - 0.5f, row * cellHeight - 0.5f, cellWidth + 1.f, cellHeight + 1.f);
						if (module->lastMoveSide == module->humanSide()) {
							nvgFillColor(args.vg, highlightGlowColor(module->highlightMode, int(20.f + 38.f * pulse)));
						}
						else {
							nvgFillColor(args.vg, nvgRGBA(255, 216, 114, int(18.f + 34.f * pulse)));
						}
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth + 1.f, row * cellHeight + 1.f, cellWidth - 2.f, cellHeight - 2.f);
						if (module->lastMoveSide == module->humanSide()) {
							nvgStrokeColor(args.vg, highlightBandColor(module->highlightMode, int(192.f + 48.f * pulse)));
						}
						else {
							nvgStrokeColor(args.vg, nvgRGB(255, 213, 79));
						}
						nvgStrokeWidth(args.vg, 2.0f);
						nvgStroke(args.vg);
					}
				}
				if (!module->gameOver && module->captureFlashSeconds > 0.f && module->lastMove.destinationIndex >= 0) {
					int row = 0;
					int col = 0;
					if (viewRowColFromBoardIndex(module->lastMove.destinationIndex, &row, &col)) {
					float alpha = clamp(module->captureFlashSeconds / 0.16f, 0.f, 1.f);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, col * cellWidth + 2.f, row * cellHeight + 2.f, cellWidth - 4.f, cellHeight - 4.f);
					nvgFillColor(args.vg, nvgRGBA(255, 210, 120, int(90.f * alpha)));
						nvgFill(args.vg);
					}
				}

					auto drawPieceAt = [&](float centerX, float centerY, int piece, float alpha) {
						if (piece == 0) {
							return;
						}
						alpha = clamp(alpha, 0.f, 1.f);
						float radius = std::min(cellWidth, cellHeight) * 0.36f;
						int fillAlpha = int(255.f * alpha);
						int strokeAlpha = int(240.f * alpha);
							int pieceSide = crownstep::pieceSide(piece);
							bool positivePiece = (pieceSide == HUMAN_SIDE);
								if (module->isOthelloMode()) {
									float discR = radius * 1.02f;
									// Othello disc colors map to board side sign:
									// +1 side = black, -1 side = white.
									NVGcolor edge = positivePiece ? nvgRGBA(28, 28, 32, strokeAlpha) : nvgRGBA(198, 200, 208, strokeAlpha);
									NVGcolor high = positivePiece ? nvgRGBA(96, 100, 114, fillAlpha) : nvgRGBA(254, 254, 255, fillAlpha);
									NVGcolor low = positivePiece ? nvgRGBA(12, 14, 18, fillAlpha) : nvgRGBA(202, 206, 216, fillAlpha);

								nvgBeginPath(args.vg);
								nvgCircle(args.vg, centerX, centerY, discR);
								NVGpaint discPaint = nvgRadialGradient(
									args.vg,
									centerX - discR * 0.24f,
									centerY - discR * 0.26f,
									discR * 0.12f,
									discR * 1.05f,
									high,
									low
								);
								nvgFillPaint(args.vg, discPaint);
								nvgFill(args.vg);
								nvgStrokeColor(args.vg, edge);
								nvgStrokeWidth(args.vg, 1.15f);
								nvgStroke(args.vg);

								nvgBeginPath(args.vg);
								nvgCircle(args.vg, centerX - discR * 0.21f, centerY - discR * 0.24f, discR * 0.28f);
								nvgFillColor(args.vg, nvgRGBA(255, 255, 255, int(40.f * alpha)));
								nvgFill(args.vg);
								return;
							}
								if (module->isChessMode()) {
									if (CHESS_ATLAS_ENABLED) {
										nvgSave(args.vg);
										nvgTranslate(args.vg, centerX, 0.f);
										nvgScale(args.vg, CHESS_HORIZONTAL_SCALE, 1.f);
										nvgTranslate(args.vg, -centerX, 0.f);
										bool drewAtlas = drawChessAtlasPiece(args.vg, centerX, centerY, cellWidth, cellHeight, piece, alpha);
										nvgRestore(args.vg);
										if (drewAtlas) {
											return;
										}
									}
									int pieceType = std::abs(piece);
									float pieceR = radius * 2.42f;
									const float chessPieceYOffset = pieceR * 0.03f;
									const float chessOutlineStroke = 1.08f;
									const float kingCrossInnerStroke = chessOutlineStroke;
									const float kingCrossOutlineStroke = 1.92f;
									NVGcolor pieceFill = positivePiece ? nvgRGBA(238, 232, 214, fillAlpha) : nvgRGBA(34, 36, 44, fillAlpha);
									NVGcolor pieceEdge = positivePiece ? nvgRGBA(76, 68, 56, strokeAlpha) : nvgRGBA(214, 220, 234, strokeAlpha);
									NVGcolor pieceDetail = positivePiece ? nvgRGBA(44, 36, 28, fillAlpha) : nvgRGBA(246, 246, 252, fillAlpha);
									NVGcolor pieceContrast = positivePiece ? nvgRGBA(24, 24, 30, strokeAlpha) : nvgRGBA(250, 250, 255, strokeAlpha);
									float baseY = centerY + pieceR * 0.29f;
									nvgSave(args.vg);
									nvgTranslate(args.vg, centerX, 0.f);
									nvgScale(args.vg, CHESS_HORIZONTAL_SCALE, 1.f);
									nvgTranslate(args.vg, -centerX, 0.f);
									nvgTranslate(args.vg, 0.f, chessPieceYOffset);

									auto fillAndStrokeCurrentPath = [&]() {
										nvgFillColor(args.vg, pieceFill);
										nvgFill(args.vg);
										nvgStrokeColor(args.vg, pieceEdge);
										nvgStrokeWidth(args.vg, chessOutlineStroke);
										nvgStroke(args.vg);
									};

									auto drawPedestal = [&]() {
										nvgBeginPath(args.vg);
										nvgRoundedRect(
											args.vg,
											centerX - pieceR * 0.42f,
											baseY,
											pieceR * 0.84f,
											pieceR * 0.12f,
											pieceR * 0.04f
										);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.35f, baseY + pieceR * 0.055f);
										nvgLineTo(args.vg, centerX + pieceR * 0.35f, baseY + pieceR * 0.055f);
										nvgStrokeColor(args.vg, pieceDetail);
										nvgStrokeWidth(args.vg, chessOutlineStroke * 0.85f);
										nvgStroke(args.vg);
									};

									auto drawTrapezoidBody = [&](float yTop, float yBottom, float halfTopW, float halfBottomW) {
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - halfBottomW, yBottom);
										nvgLineTo(args.vg, centerX - halfTopW, yTop);
										nvgLineTo(args.vg, centerX + halfTopW, yTop);
										nvgLineTo(args.vg, centerX + halfBottomW, yBottom);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
									};

									switch (pieceType) {
									case crownstep::CHESS_PAWN: {
										drawPedestal();
										nvgBeginPath(args.vg);
										nvgCircle(args.vg, centerX, centerY - pieceR * 0.26f, pieceR * 0.14f);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.20f, baseY);
										nvgBezierTo(args.vg,
											centerX - pieceR * 0.20f, centerY - pieceR * 0.01f,
											centerX - pieceR * 0.10f, centerY - pieceR * 0.17f,
											centerX, centerY - pieceR * 0.17f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.10f, centerY - pieceR * 0.17f,
											centerX + pieceR * 0.20f, centerY - pieceR * 0.01f,
											centerX + pieceR * 0.20f, baseY);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
										break;
									}
									case crownstep::CHESS_ROOK: {
										drawPedestal();
										drawTrapezoidBody(centerY - pieceR * 0.32f, baseY, pieceR * 0.20f, pieceR * 0.29f);
										nvgBeginPath(args.vg);
										nvgRoundedRect(
											args.vg,
											centerX - pieceR * 0.32f,
											centerY - pieceR * 0.42f,
											pieceR * 0.64f,
											pieceR * 0.12f,
											pieceR * 0.02f
										);
										fillAndStrokeCurrentPath();
										for (int i = -1; i <= 1; ++i) {
											float notchX = centerX + float(i) * pieceR * 0.15f;
											nvgBeginPath(args.vg);
											nvgRect(args.vg, notchX - pieceR * 0.045f, centerY - pieceR * 0.42f, pieceR * 0.09f, pieceR * 0.05f);
											nvgFillColor(args.vg, pieceContrast);
											nvgFill(args.vg);
										}
										break;
									}
									case crownstep::CHESS_KNIGHT: {
										drawPedestal();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.31f, baseY);
										nvgBezierTo(args.vg,
											centerX - pieceR * 0.31f, centerY - pieceR * 0.18f,
											centerX - pieceR * 0.15f, centerY - pieceR * 0.42f,
											centerX + pieceR * 0.03f, centerY - pieceR * 0.50f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.22f, centerY - pieceR * 0.57f,
											centerX + pieceR * 0.31f, centerY - pieceR * 0.39f,
											centerX + pieceR * 0.20f, centerY - pieceR * 0.31f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.12f, centerY - pieceR * 0.26f,
											centerX + pieceR * 0.10f, centerY - pieceR * 0.18f,
											centerX + pieceR * 0.16f, centerY - pieceR * 0.10f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.08f, centerY + pieceR * 0.05f,
											centerX - pieceR * 0.08f, centerY + pieceR * 0.03f,
											centerX - pieceR * 0.16f, centerY - pieceR * 0.05f);
										nvgLineTo(args.vg, centerX - pieceR * 0.22f, baseY);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgCircle(args.vg, centerX + pieceR * 0.08f, centerY - pieceR * 0.38f, pieceR * 0.030f);
										nvgFillColor(args.vg, pieceContrast);
										nvgFill(args.vg);
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.15f, centerY - pieceR * 0.10f);
										nvgLineTo(args.vg, centerX + pieceR * 0.02f, centerY - pieceR * 0.20f);
										nvgStrokeColor(args.vg, pieceDetail);
										nvgStrokeWidth(args.vg, chessOutlineStroke);
										nvgStroke(args.vg);
										break;
									}
									case crownstep::CHESS_BISHOP: {
										drawPedestal();
										nvgBeginPath(args.vg);
										nvgCircle(args.vg, centerX, centerY - pieceR * 0.46f, pieceR * 0.07f);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX, centerY - pieceR * 0.40f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.22f, centerY - pieceR * 0.26f,
											centerX + pieceR * 0.21f, centerY - pieceR * 0.03f,
											centerX, baseY);
										nvgBezierTo(args.vg,
											centerX - pieceR * 0.21f, centerY - pieceR * 0.03f,
											centerX - pieceR * 0.22f, centerY - pieceR * 0.26f,
											centerX, centerY - pieceR * 0.40f);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.09f, centerY - pieceR * 0.24f);
										nvgLineTo(args.vg, centerX + pieceR * 0.09f, centerY - pieceR * 0.08f);
										nvgStrokeColor(args.vg, pieceDetail);
										nvgStrokeWidth(args.vg, chessOutlineStroke);
										nvgStroke(args.vg);
										break;
									}
									case crownstep::CHESS_QUEEN: {
										drawPedestal();
										drawTrapezoidBody(centerY - pieceR * 0.29f, baseY, pieceR * 0.11f, pieceR * 0.30f);
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.35f, centerY - pieceR * 0.31f);
										nvgLineTo(args.vg, centerX - pieceR * 0.24f, centerY - pieceR * 0.49f);
										nvgLineTo(args.vg, centerX - pieceR * 0.10f, centerY - pieceR * 0.34f);
										nvgLineTo(args.vg, centerX, centerY - pieceR * 0.54f);
										nvgLineTo(args.vg, centerX + pieceR * 0.10f, centerY - pieceR * 0.34f);
										nvgLineTo(args.vg, centerX + pieceR * 0.24f, centerY - pieceR * 0.49f);
										nvgLineTo(args.vg, centerX + pieceR * 0.35f, centerY - pieceR * 0.31f);
										nvgLineTo(args.vg, centerX + pieceR * 0.30f, centerY - pieceR * 0.23f);
										nvgLineTo(args.vg, centerX - pieceR * 0.30f, centerY - pieceR * 0.23f);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
										for (int i = -2; i <= 2; ++i) {
											float cx = centerX + float(i) * pieceR * 0.12f;
											float cy = centerY - pieceR * (0.55f - std::abs(float(i)) * 0.06f);
											nvgBeginPath(args.vg);
											nvgCircle(args.vg, cx, cy, pieceR * 0.038f);
											fillAndStrokeCurrentPath();
										}
										break;
									}
									case crownstep::CHESS_KING:
									default: {
										drawPedestal();
										drawTrapezoidBody(centerY - pieceR * 0.34f, baseY, pieceR * 0.13f, pieceR * 0.28f);
										nvgBeginPath(args.vg);
										nvgRoundedRect(
											args.vg,
											centerX - pieceR * 0.15f,
											centerY - pieceR * 0.44f,
											pieceR * 0.30f,
											pieceR * 0.10f,
											pieceR * 0.03f
										);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.13f, centerY - pieceR * 0.22f);
										nvgLineTo(args.vg, centerX + pieceR * 0.13f, centerY - pieceR * 0.22f);
										nvgStrokeColor(args.vg, pieceDetail);
										nvgStrokeWidth(args.vg, chessOutlineStroke);
										nvgStroke(args.vg);

										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX, centerY - pieceR * 0.66f);
										nvgLineTo(args.vg, centerX, centerY - pieceR * 0.40f);
										nvgMoveTo(args.vg, centerX - pieceR * 0.13f, centerY - pieceR * 0.53f);
										nvgLineTo(args.vg, centerX + pieceR * 0.13f, centerY - pieceR * 0.53f);
										nvgStrokeColor(args.vg, pieceContrast);
										nvgStrokeWidth(args.vg, kingCrossOutlineStroke);
										nvgStroke(args.vg);

										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX, centerY - pieceR * 0.66f);
										nvgLineTo(args.vg, centerX, centerY - pieceR * 0.40f);
										nvgMoveTo(args.vg, centerX - pieceR * 0.13f, centerY - pieceR * 0.53f);
										nvgLineTo(args.vg, centerX + pieceR * 0.13f, centerY - pieceR * 0.53f);
										NVGcolor crossInner = positivePiece ? nvgRGBA(246, 246, 252, strokeAlpha) : nvgRGBA(18, 18, 24, strokeAlpha);
										nvgStrokeColor(args.vg, crossInner);
										nvgStrokeWidth(args.vg, kingCrossInnerStroke);
										nvgStroke(args.vg);
										break;
									}
									}
									nvgRestore(args.vg);
									return;
								}

						// Checkers palette is tied to board side sign so Cause/Effect can
						// swap piece color while rotation still keeps the player at bottom.
						// + side = red pieces, - side = black pieces.
						NVGcolor coreInner = positivePiece ? nvgRGBA(237, 112, 94, fillAlpha) : nvgRGBA(78, 78, 86, fillAlpha);
						NVGcolor coreOuter = positivePiece ? nvgRGBA(152, 46, 38, fillAlpha) : nvgRGBA(12, 12, 16, fillAlpha);
						NVGcolor rimBright = positivePiece ? nvgRGBA(255, 228, 208, strokeAlpha) : nvgRGBA(210, 210, 216, strokeAlpha);
						NVGcolor rimDark = positivePiece ? nvgRGBA(82, 22, 16, int(210.f * alpha)) : nvgRGBA(6, 6, 9, int(215.f * alpha));

						// Base disc with beveled radial falloff.
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, radius);
						NVGpaint corePaint = nvgRadialGradient(args.vg, centerX - radius * 0.18f, centerY - radius * 0.2f,
							radius * 0.14f, radius * 1.06f, coreInner, coreOuter);
						nvgFillPaint(args.vg, corePaint);
						nvgFill(args.vg);

						// Outer and inner rim lines for a machined/checker edge feel.
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, radius);
						nvgStrokeColor(args.vg, rimBright);
						nvgStrokeWidth(args.vg, 1.55f);
						nvgStroke(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, radius * 0.83f);
						nvgStrokeColor(args.vg, rimDark);
						nvgStrokeWidth(args.vg, 1.05f);
						nvgStroke(args.vg);

						// Checker-like stacked rim: outer annulus plus discrete radial ridge facets.
						float rimOuterR = radius * 1.01f;
						float rimInnerR = radius * 0.80f;
						NVGcolor rimBandColor = positivePiece ? nvgRGBA(112, 38, 30, int(128.f * alpha))
						                                      : nvgRGBA(26, 26, 32, int(146.f * alpha));
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, rimOuterR);
						nvgCircle(args.vg, centerX, centerY, rimInnerR);
						nvgPathWinding(args.vg, NVG_HOLE);
						nvgFillColor(args.vg, rimBandColor);
						nvgFill(args.vg);

						const int ridgeCount = 32;
						float step = 2.f * float(M_PI) / float(ridgeCount);
						float ridgeSpan = step * 0.56f;
						float ridgeInnerR = radius * 0.86f;
						float ridgeOuterR = radius * 1.00f;
						for (int ridge = 0; ridge < ridgeCount; ++ridge) {
							float aMid = float(ridge) * step;
							float a0 = aMid - ridgeSpan * 0.5f;
							float a1 = aMid + ridgeSpan * 0.5f;
							float c0 = std::cos(a0);
							float s0 = std::sin(a0);
							float c1 = std::cos(a1);
							float s1 = std::sin(a1);
							float x0i = centerX + c0 * ridgeInnerR;
							float y0i = centerY + s0 * ridgeInnerR;
							float x1i = centerX + c1 * ridgeInnerR;
							float y1i = centerY + s1 * ridgeInnerR;
							float x1o = centerX + c1 * ridgeOuterR;
							float y1o = centerY + s1 * ridgeOuterR;
							float x0o = centerX + c0 * ridgeOuterR;
							float y0o = centerY + s0 * ridgeOuterR;

							NVGcolor ridgeFillA = positivePiece ? nvgRGBA(248, 176, 154, int(112.f * alpha))
							                                    : nvgRGBA(172, 172, 184, int(94.f * alpha));
							NVGcolor ridgeFillB = positivePiece ? nvgRGBA(198, 104, 86, int(104.f * alpha))
							                                    : nvgRGBA(112, 112, 122, int(86.f * alpha));
							NVGcolor ridgeStroke = positivePiece ? nvgRGBA(86, 24, 18, int(110.f * alpha))
							                                     : nvgRGBA(8, 8, 12, int(116.f * alpha));

							nvgBeginPath(args.vg);
							nvgMoveTo(args.vg, x0i, y0i);
							nvgLineTo(args.vg, x1i, y1i);
							nvgLineTo(args.vg, x1o, y1o);
							nvgLineTo(args.vg, x0o, y0o);
							nvgClosePath(args.vg);
							nvgFillColor(args.vg, (ridge & 1) ? ridgeFillA : ridgeFillB);
							nvgFill(args.vg);
							nvgStrokeColor(args.vg, ridgeStroke);
							nvgStrokeWidth(args.vg, 0.34f);
							nvgStroke(args.vg);
						}

						// Tie the rim together with thin contour rings.
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, rimOuterR);
						nvgStrokeColor(args.vg, positivePiece ? nvgRGBA(255, 212, 196, int(60.f * alpha))
						                                      : nvgRGBA(196, 196, 206, int(46.f * alpha)));
						nvgStrokeWidth(args.vg, 0.70f);
						nvgStroke(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, rimInnerR);
						nvgStrokeColor(args.vg, positivePiece ? nvgRGBA(70, 18, 14, int(82.f * alpha))
						                                      : nvgRGBA(6, 6, 10, int(96.f * alpha)));
						nvgStrokeWidth(args.vg, 0.64f);
						nvgStroke(args.vg);

						// Top sheen.
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX - radius * 0.20f, centerY - radius * 0.24f, radius * 0.34f);
						nvgFillColor(args.vg, nvgRGBA(255, 255, 255, int(36.f * alpha)));
						nvgFill(args.vg);

						if (crownstep::pieceIsKing(piece)) {
							// Ultra-simple crown: mostly gold, minimal edging, wider outer points.
							NVGcolor crownEdge = nvgRGBA(172, 126, 40, int(132.f * alpha));
							NVGcolor crownFillTop = nvgRGBA(255, 238, 172, int(246.f * alpha));
							NVGcolor crownFillBottom = nvgRGBA(236, 184, 70, int(244.f * alpha));
							float crownYOffset = radius * 0.07f;
							float leftX = centerX - radius * 0.56f;
							float rightX = centerX + radius * 0.56f;
							float bandTopY = centerY + radius * 0.05f + crownYOffset;
							float bandBottomY = centerY + radius * 0.24f + crownYOffset;
							float sideTipY = centerY - radius * 0.36f + crownYOffset;
							float centerTipY = centerY - radius * 0.55f + crownYOffset;
							float valleyY = centerY - radius * 0.08f + crownYOffset;
							float leftTipX = centerX - radius * 0.50f;
							float rightTipX = centerX + radius * 0.50f;
							float leftValleyX = centerX - radius * 0.18f;
							float rightValleyX = centerX + radius * 0.18f;

							nvgBeginPath(args.vg);
							nvgMoveTo(args.vg, leftX, bandTopY);
							nvgLineTo(args.vg, leftTipX, sideTipY);
							nvgLineTo(args.vg, leftValleyX, valleyY);
							nvgLineTo(args.vg, centerX, centerTipY);
							nvgLineTo(args.vg, rightValleyX, valleyY);
							nvgLineTo(args.vg, rightTipX, sideTipY);
							nvgLineTo(args.vg, rightX, bandTopY);
							nvgClosePath(args.vg);
							NVGpaint crownBodyPaint = nvgLinearGradient(
								args.vg, centerX, centerY - radius * 0.58f, centerX, bandBottomY, crownFillTop, crownFillBottom);
							nvgFillPaint(args.vg, crownBodyPaint);
							nvgFill(args.vg);
							nvgStrokeColor(args.vg, crownEdge);
							nvgStrokeWidth(args.vg, 0.62f);
							nvgStroke(args.vg);

							nvgBeginPath(args.vg);
							nvgRoundedRect(args.vg, leftX - radius * 0.03f, bandTopY, (rightX - leftX) + radius * 0.06f,
								bandBottomY - bandTopY, radius * 0.06f);
							nvgFillColor(args.vg, nvgRGBA(230, 174, 58, int(244.f * alpha)));
							nvgFill(args.vg);
							nvgStrokeColor(args.vg, crownEdge);
							nvgStrokeWidth(args.vg, 0.58f);
							nvgStroke(args.vg);

							// Three tip beads improve recognition at distance.
							for (int i = 0; i < 3; ++i) {
								float tipX = (i == 0) ? leftTipX : ((i == 1) ? centerX : rightTipX);
								float tipY = (i == 1) ? centerTipY : sideTipY;
								float beadR = radius * 0.07f;
								nvgBeginPath(args.vg);
								nvgCircle(args.vg, tipX, tipY, beadR);
								nvgFillColor(args.vg, nvgRGBA(255, 223, 120, int(246.f * alpha)));
								nvgFill(args.vg);
								nvgStrokeColor(args.vg, crownEdge);
								nvgStrokeWidth(args.vg, 0.55f);
								nvgStroke(args.vg);
							}

						}
					};

					auto indexIsQueuedDestination = [&](int index) {
						for (const Crownstep::MoveVisualAnimation& queued : module->moveAnimationQueue) {
							if (queued.destinationIndex == index) {
								return true;
							}
						}
						return false;
					};
					auto drawPieceAtScaled = [&](float centerX, float centerY, int piece, float alpha, float scaleX, float scaleY) {
						nvgSave(args.vg);
						nvgTranslate(args.vg, centerX, centerY);
						nvgScale(args.vg, scaleX, scaleY);
						nvgTranslate(args.vg, -centerX, -centerY);
						drawPieceAt(centerX, centerY, piece, alpha);
						nvgRestore(args.vg);
					};
					auto isActiveAnimatedCaptureIndex = [&](int index) {
						if (!module->isOthelloMode() || !module->moveAnimation.active) {
							return false;
						}
						for (int captureIndex : module->moveAnimation.capturedIndices) {
							if (captureIndex == index) {
								return true;
							}
						}
						return false;
					};

						const bool highlightActive =
							!module->gameOver
							&& module->turnSide == module->humanSide()
							&& module->highlightMode != Crownstep::HIGHLIGHT_OFF;
						const bool showMovablePieceRingHints =
							highlightActive;
						int cellCount = module->boardCellCount();
						std::vector<uint8_t> movableOrigins(size_t(cellCount), 0u);
						if (highlightActive) {
							for (const Move& move : module->humanMoves) {
								if (move.originIndex >= 0 && move.originIndex < cellCount) {
									movableOrigins[size_t(move.originIndex)] = 1u;
								}
							}
						}
						const bool selectedSquareHighlightActive = !module->gameOver && module->selectedSquare >= 0;
						const float sharedRingPulse = 0.5f + 0.5f * std::sin(animTime * 4.6f + 0.4f);
						const float sharedOpponentRingPulse = 0.5f + 0.5f * std::sin(animTime * 4.6f + 1.6f);

						for (int i = 0; i < cellCount; ++i) {
							int piece = module->board[size_t(i)];
							if (piece == 0) {
							continue;
						}
						if (isActiveAnimatedCaptureIndex(i)) {
							continue;
						}
						if ((module->moveAnimation.active && i == module->moveAnimation.destinationIndex) || indexIsQueuedDestination(i)) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!viewRowColFromBoardIndex(i, &row, &col)) {
						continue;
					}
							float centerX = (col + 0.5f) * cellWidth;
							float centerY = (row + 0.5f) * cellHeight;
							drawPieceAt(centerX, centerY, piece, 1.f);
						}

					// Staged queued moves: keep the queued moving piece visible at its start cell
					// until that queued animation becomes active, preventing destination teleport.
					for (const Crownstep::MoveVisualAnimation& queued : module->moveAnimationQueue) {
						if (queued.path.empty() || queued.movingPiece == 0) {
							continue;
						}
						int startIndex = queued.path.front();
						int row = 0;
						int col = 0;
						if (!viewRowColFromBoardIndex(startIndex, &row, &col)) {
							continue;
						}
						drawPieceAt((col + 0.5f) * cellWidth, (row + 0.5f) * cellHeight, queued.movingPiece, 0.95f);
					}

				if (module->moveAnimation.active && module->moveAnimation.path.size() >= 2) {
					float duration = std::max(module->moveAnimation.durationSeconds, 1e-6f);
					float t = clamp(module->moveAnimation.elapsedSeconds / duration, 0.f, 1.f);

					for (size_t i = 0; i < module->moveAnimation.capturedIndices.size(); ++i) {
						int captureIndex = module->moveAnimation.capturedIndices[i];
						int capturedPiece = (i < module->moveAnimation.capturedPieces.size()) ? module->moveAnimation.capturedPieces[i] : 0;
						if (capturedPiece == 0) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!viewRowColFromBoardIndex(captureIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						if (module->isOthelloMode()) {
							int flippedPiece = module->board[size_t(captureIndex)];
							if (flippedPiece == 0) {
								flippedPiece = -capturedPiece;
							}
							float flipStart = OTHELLO_FLIP_SECONDS_PER_PIECE * float(i);
							float flipEnd = flipStart + OTHELLO_FLIP_SECONDS_PER_PIECE;
							if (module->moveAnimation.elapsedSeconds <= flipStart) {
								drawPieceAt(centerX, centerY, capturedPiece, 1.f);
								continue;
							}
							if (module->moveAnimation.elapsedSeconds >= flipEnd) {
								drawPieceAt(centerX, centerY, flippedPiece, 1.f);
								continue;
							}
							float local = clamp(
								(module->moveAnimation.elapsedSeconds - flipStart) / OTHELLO_FLIP_SECONDS_PER_PIECE,
								0.f,
								1.f
							);
							float firstHalf = clamp(local * 2.f, 0.f, 1.f);
							float secondHalf = clamp((local - 0.5f) * 2.f, 0.f, 1.f);
							float widthScale = std::max(0.06f, std::fabs(std::cos(local * float(M_PI))));
							if (local < 0.5f) {
								drawPieceAtScaled(centerX, centerY, capturedPiece, 1.f - 0.2f * firstHalf, widthScale, 1.f);
							}
							else {
								drawPieceAtScaled(centerX, centerY, flippedPiece, 0.8f + 0.2f * secondHalf, widthScale, 1.f);
							}
						}
						else {
							float ghostAlpha = clamp(0.72f - t * 1.45f, 0.f, 0.72f);
							drawPieceAt(centerX, centerY, capturedPiece, ghostAlpha);
						}
					}

					int segments = int(module->moveAnimation.path.size()) - 1;
					float segmentPos = t * float(segments);
					int segmentIndex = clamp(int(std::floor(segmentPos)), 0, std::max(segments - 1, 0));
					float localT = segmentPos - float(segmentIndex);
					if (segmentIndex >= segments) {
						segmentIndex = segments - 1;
						localT = 1.f;
					}

					int fromIndex = module->moveAnimation.path[size_t(segmentIndex)];
					int toIndex = module->moveAnimation.path[size_t(segmentIndex + 1)];
					int fromRow = 0;
					int fromCol = 0;
					int toRow = 0;
					int toCol = 0;
					if (viewRowColFromBoardIndex(fromIndex, &fromRow, &fromCol) &&
						viewRowColFromBoardIndex(toIndex, &toRow, &toCol)) {
						float fromX = (fromCol + 0.5f) * cellWidth;
						float fromY = (fromRow + 0.5f) * cellHeight;
						float toX = (toCol + 0.5f) * cellWidth;
						float toY = (toRow + 0.5f) * cellHeight;
						float centerX = fromX + (toX - fromX) * localT;
						float centerY = fromY + (toY - fromY) * localT;
						float shadowRadius = std::min(cellWidth, cellHeight) * 0.40f;

						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX + 0.9f, centerY + 1.2f, shadowRadius);
						nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 42));
						nvgFill(args.vg);

						drawPieceAt(centerX, centerY, module->moveAnimation.movingPiece, 1.f);
					}
				}

					// Keep forced/destination hint visibility even when a destination is visually occupied
					// (e.g. during queued animations) by drawing a top-layer pulse over the piece.
					if (!module->gameOver && module->highlightMode != Crownstep::HIGHLIGHT_OFF) {
						for (int destinationIndex : module->highlightedDestinations) {
							if (destinationIndex < 0 || destinationIndex >= cellCount) {
								continue;
							}
						bool occupied = module->board[size_t(destinationIndex)] != 0;
						bool landingAnimation =
							(module->moveAnimation.active && module->moveAnimation.destinationIndex == destinationIndex)
							|| indexIsQueuedDestination(destinationIndex);
						if (!occupied && !landingAnimation) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!viewRowColFromBoardIndex(destinationIndex, &row, &col)) {
							continue;
						}
							float centerX = (col + 0.5f) * cellWidth;
							float centerY = (row + 0.5f) * cellHeight;
							float breath = showMovablePieceRingHints
								? sharedRingPulse
								: (0.5f + 0.5f * std::sin(animTime * 4.8f + float(destinationIndex) * 0.43f + 0.9f));
							float ringRadius = std::min(cellWidth, cellHeight) * (0.21f + 0.05f * breath);
							nvgBeginPath(args.vg);
							nvgCircle(args.vg, centerX, centerY, ringRadius);
						nvgStrokeColor(args.vg, highlightBandColor(module->highlightMode, int(186.f + 62.f * breath)));
						nvgStrokeWidth(args.vg, 1.9f);
						nvgStroke(args.vg);
					}

						if (renderOpponentMoveHints) for (int destinationIndex : module->opponentHighlightedDestinations) {
							bool overlapsHuman = false;
						for (int humanDestination : module->highlightedDestinations) {
							if (humanDestination == destinationIndex) {
								overlapsHuman = true;
								break;
							}
						}
							if (overlapsHuman || destinationIndex < 0 || destinationIndex >= cellCount) {
								continue;
							}
						if (module->opponentHintsPreviewActive && module->board[size_t(destinationIndex)] != 0) {
							continue;
						}
						bool occupied = module->board[size_t(destinationIndex)] != 0;
						bool landingAnimation =
							(module->moveAnimation.active && module->moveAnimation.destinationIndex == destinationIndex)
							|| indexIsQueuedDestination(destinationIndex);
						if (!occupied && !landingAnimation) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!viewRowColFromBoardIndex(destinationIndex, &row, &col)) {
							continue;
						}
							float centerX = (col + 0.5f) * cellWidth;
							float centerY = (row + 0.5f) * cellHeight;
							float breath = showMovablePieceRingHints
								? sharedOpponentRingPulse
								: (0.5f + 0.5f * std::sin(animTime * 4.2f + float(destinationIndex) * 0.39f + 2.2f));
							float ringRadius = std::min(cellWidth, cellHeight) * (0.205f + 0.05f * breath);
							nvgBeginPath(args.vg);
							nvgCircle(args.vg, centerX, centerY, ringRadius);
						nvgStrokeColor(args.vg, highlightBandColor(module->highlightMode, int(180.f + 68.f * breath)));
						nvgStrokeWidth(args.vg, 1.85f);
						nvgStroke(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.062f);
						nvgFillColor(args.vg, highlightGlowColor(module->highlightMode, int(190.f + 56.f * breath)));
						nvgFill(args.vg);
					}
				}

						// Human-turn assist ring mode: pulse rings on movable pieces.
							if (showMovablePieceRingHints) {
							for (int i = 0; i < cellCount; ++i) {
							if (!movableOrigins[size_t(i)]) {
								continue;
							}
							if (selectedSquareHighlightActive && i == module->selectedSquare) {
								continue;
							}
							int piece = module->board[size_t(i)];
							if (crownstep::pieceSide(piece) != module->humanSide()) {
								continue;
							}
							int row = 0;
							int col = 0;
							if (!viewRowColFromBoardIndex(i, &row, &col)) {
								continue;
								}
									float centerX = (col + 0.5f) * cellWidth;
									float centerY = (row + 0.5f) * cellHeight;
									float pulse = sharedRingPulse;
									bool useChessFallbackContour = false;
								if (module->isChessMode()) {
									bool drewContour = false;
									if (CHESS_ATLAS_ENABLED) {
										nvgSave(args.vg);
										nvgTranslate(args.vg, centerX, 0.f);
										nvgScale(args.vg, CHESS_HORIZONTAL_SCALE, 1.f);
										nvgTranslate(args.vg, -centerX, 0.f);
										drewContour = drawChessAtlasPieceRingContour(
											args.vg,
											centerX,
											centerY,
											cellWidth,
											cellHeight,
											piece,
											module->highlightMode,
											pulse
										);
										nvgRestore(args.vg);
									}
									if (drewContour) {
										// Keep contour as an outline by repainting the normal piece on top.
										drawPieceAt(centerX, centerY, piece, 1.f);
										continue;
									}
									useChessFallbackContour = true;
								}
								if (useChessFallbackContour) {
									float minCell = std::min(cellWidth, cellHeight);
									float ringW = minCell * 0.64f;
									float ringH = minCell * 0.95f;
									float ringX = centerX - ringW * 0.5f;
									float ringY = centerY - ringH * 0.58f;
									float corner = minCell * 0.18f;
									float outerStroke = std::max(2.1f, minCell * 0.115f);
									float innerStroke = std::max(1.5f, minCell * 0.082f);
									// Fixed contour shell.
									nvgBeginPath(args.vg);
									nvgRoundedRect(args.vg, ringX, ringY, ringW, ringH, corner);
									nvgStrokeColor(args.vg, highlightShellColor(module->highlightMode, 172));
									nvgStrokeWidth(args.vg, outerStroke);
									nvgStroke(args.vg);
									// Fixed bright band keeps contour solid at all pulse phases.
									nvgBeginPath(args.vg);
									nvgRoundedRect(args.vg, ringX, ringY, ringW, ringH, corner);
									nvgStrokeColor(args.vg, highlightBandColor(module->highlightMode, 244));
									nvgStrokeWidth(args.vg, innerStroke);
									nvgStroke(args.vg);
									continue;
								}
								float ringRadius = std::min(cellWidth, cellHeight) * (0.43f + 0.03f * pulse);

							nvgBeginPath(args.vg);
							nvgCircle(args.vg, centerX, centerY, ringRadius);
							nvgStrokeColor(args.vg, highlightGlowColor(module->highlightMode, int(38.f + 44.f * pulse)));
							nvgStrokeWidth(args.vg, 3.6f);
							nvgStroke(args.vg);

							nvgBeginPath(args.vg);
							nvgCircle(args.vg, centerX, centerY, ringRadius);
							nvgStrokeColor(args.vg, highlightBandColor(module->highlightMode, int(182.f + 58.f * pulse)));
							nvgStrokeWidth(args.vg, 1.85f);
							nvgStroke(args.vg);
						}
					}

					if (module->gameOver) {
					nvgBeginPath(args.vg);
					nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
				nvgFillColor(args.vg, nvgRGBA(8, 8, 10, 92));
				nvgFill(args.vg);
				nvgBeginPath(args.vg);
				nvgRect(args.vg, 0.f, box.size.y * 0.39f, box.size.x, box.size.y * 0.22f);
				nvgFillColor(args.vg, nvgRGBA(10, 10, 12, 180));
				nvgFill(args.vg);
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
				nvgFontSize(args.vg, 15.f);
				nvgFillColor(args.vg, nvgRGB(244, 229, 206));
				nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.47f, "Game Complete", nullptr);
					nvgFontSize(args.vg, 10.5f);
					nvgFillColor(args.vg, nvgRGB(213, 189, 160));
					nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.545f, "Playback Mode", nullptr);
				}

					// Always render value labels last so they sit on top of all board visuals.
					drawCellPitchOverlay();
			}
		nvgRestore(args.vg);
	}
};

struct CrownRibbonWidget final : OpaqueWidget {
	Crownstep* module = nullptr;
	int lastRecentCap = 16;
	Vec lastHoverPos = Vec(0.f, 0.f);
	bool capDragActive = false;
	bool capDragTrimMode = false;
	Vec capDragLocal = Vec(0.f, 0.f);

	enum class VisualMode {
		DISCRETE,
		DOUBLE_ROW,
		CHUNKED_64,
		COMPRESSED
	};

	struct RibbonState {
		int historySize = 0;
		int activeStart = 0;
		int activeLength = 0;
		int playbackIndex = -1;
		int capValue = 0; // 0 means FULL
		bool fullMode = true;
		float eventFlash = 0.f;
		std::vector<float> stepWeight;
	};

	struct RibbonLayout {
		bool compact = false;
		float stripX = 0.f;
		float stripW = 0.f;
		float historyY = 0.f;
		float historyH = 0.f;
		float loopY = 0.f;
		float loopH = 0.f;
	};

	static constexpr int PRESET_COUNT = 4;
	static constexpr int PRESET_CAP_VALUES[PRESET_COUNT] = {0, 8, 16, 32};

	explicit CrownRibbonWidget(Crownstep* crownstepModule) {
		module = crownstepModule;
	}

	VisualMode chooseMode(int activeLength) const {
		if (activeLength <= 64) {
			return VisualMode::DISCRETE;
		}
		if (activeLength <= 128) {
			return VisualMode::DOUBLE_ROW;
		}
		return VisualMode::CHUNKED_64;
	}

	RibbonLayout computeLayout() const {
		RibbonLayout layout;
		const float h = box.size.y;
		const float w = box.size.x;
		const float pad = 0.f;
		layout.compact = (h <= 23.f) || ((w / std::max(1.f, h)) >= 5.f && h <= 28.f);
		// Reserve extra headroom in non-compact mode so the crown cap above
		// the history playhead stays fully inside the widget bounds.
		float ribbonTop = layout.compact ? 0.9f : 5.2f;
		float ribbonBottom = h - 1.0f;
		float ribbonH = std::max(4.f, ribbonBottom - ribbonTop);
		float stripGap = 0.8f;
		float stripHalfH = std::max(2.2f, (ribbonH - stripGap) * 0.5f);
		layout.stripX = pad;
		layout.stripW = std::max(6.f, w - 2.f * pad);
		layout.historyY = ribbonTop;
		layout.historyH = stripHalfH;
		layout.loopY = layout.historyY + layout.historyH + stripGap;
		layout.loopH = stripHalfH;
		return layout;
	}

	bool pointInHistoryStrip(Vec localPos, const RibbonLayout& layout) const {
		return localPos.x >= layout.stripX
			&& localPos.x <= (layout.stripX + layout.stripW)
			&& localPos.y >= layout.historyY
			&& localPos.y <= (layout.historyY + layout.historyH);
	}

	bool pointInLoopStrip(Vec localPos, const RibbonLayout& layout) const {
		return localPos.x >= layout.stripX
			&& localPos.x <= (layout.stripX + layout.stripW)
			&& localPos.y >= layout.loopY
			&& localPos.y <= (layout.loopY + layout.loopH);
	}

	float loopButtonWidth(const RibbonLayout& layout) const {
		return clamp(layout.loopH * 1.7f, layout.compact ? 7.f : 8.f, layout.compact ? 10.f : 13.f);
	}

	float loopButtonGap(const RibbonLayout& layout) const {
		return layout.compact ? 1.0f : 1.4f;
	}

	bool pointInLoopMinusButton(Vec localPos, const RibbonLayout& layout) const {
		if (!pointInLoopStrip(localPos, layout)) {
			return false;
		}
		float buttonW = loopButtonWidth(layout);
		return localPos.x <= (layout.stripX + buttonW);
	}

	bool pointInLoopPlusButton(Vec localPos, const RibbonLayout& layout) const {
		if (!pointInLoopStrip(localPos, layout)) {
			return false;
		}
		float buttonW = loopButtonWidth(layout);
		return localPos.x >= (layout.stripX + layout.stripW - buttonW);
	}

	float loopDataX(const RibbonLayout& layout) const {
		float buttonW = loopButtonWidth(layout);
		float gap = loopButtonGap(layout);
		return layout.stripX + buttonW + gap;
	}

	float loopDataW(const RibbonLayout& layout) const {
		float buttonW = loopButtonWidth(layout);
		float gap = loopButtonGap(layout);
		return std::max(6.f, layout.stripW - 2.f * (buttonW + gap));
	}

	Vec currentLocalMousePos() const {
		if (!parent || !APP || !APP->scene || !APP->scene->rack) {
			return lastHoverPos;
		}
		return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
	}

	int clipCountForLocalX(float localX, int historySize, const RibbonLayout& layout) const {
		if (historySize <= 0) {
			return 0;
		}
		// For short histories, let FULL and LIVE GAME use their natural state
		// widths. Once those become too narrow to hit comfortably, enforce a
		// minimum edge zone.
		float naturalStateW = layout.stripW / float(std::max(1, historySize));
		float minFullZonePx = clamp(layout.stripW * 0.035f, 5.f, 14.f);
		float minLiveZonePx = clamp(layout.stripW * 0.030f, 4.f, 11.f);
		float fullZonePx = std::min(layout.stripW, std::max(naturalStateW, minFullZonePx));
		float liveZonePx = 0.f;
		if (historySize > 1) {
			liveZonePx = std::min(std::max(0.f, layout.stripW - fullZonePx), std::max(naturalStateW, minLiveZonePx));
		}
		if (localX <= layout.stripX + fullZonePx) {
			return 0; // Full
		}
		if (historySize > 1 && localX >= (layout.stripX + layout.stripW - liveZonePx)) {
			return 1; // Live Game
		}
		float usableW = std::max(1.f, layout.stripW - fullZonePx - liveZonePx);
		float norm = clamp((localX - (layout.stripX + fullZonePx)) / usableW, 0.f, 1.f);
		const int maxRecent = std::max(1, historySize - 1);
		const int middleStateCount = std::max(0, maxRecent - 1); // counts 2..maxRecent
		if (middleStateCount <= 0) {
			return maxRecent;
		}
		int bucket = clamp(int(std::floor((1.f - norm) * float(middleStateCount))), 0, middleStateCount - 1);
		return clamp(2 + bucket, 2, maxRecent);
	}

	void formatWindowPreviewText(int clipCount, int historySize, char* outText, size_t outSize) const {
		if (!outText || outSize == 0) {
			return;
		}
		if (clipCount <= 0 || clipCount >= historySize) {
			std::snprintf(outText, outSize, "Window: Full");
			return;
		}
		if (clipCount <= 1) {
			std::snprintf(outText, outSize, "Window: Live Game");
			return;
		}
		std::snprintf(outText, outSize, "Window: %d", clipCount);
	}

	int nudgedClipCountPreview(int dir, int historySize) const {
		if (!module || historySize <= 0 || dir == 0) {
			return capValueFromModule();
		}
		int cap = capValueFromModule();
		if (cap <= 0) {
			return (dir < 0) ? std::max(1, historySize - 1) : 0;
		}
		int next = cap + dir;
		if (next >= historySize) {
			return 0;
		}
		return clamp(next, 1, std::max(1, historySize - 1));
	}

	void applyClipCount(int clipCount) {
		if (!module) {
			return;
		}
		if (clipCount <= 0) {
			module->sequenceCapOverride = 0;
			module->params[Crownstep::SEQ_LENGTH_PARAM].setValue(float(SEQ_LENGTH_MAX));
		}
		else {
			int clipped = std::max(1, clipCount);
			lastRecentCap = clipped;
			if (clipped <= (SEQ_LENGTH_MAX - 1)) {
				module->sequenceCapOverride = -1;
				module->params[Crownstep::SEQ_LENGTH_PARAM].setValue(float(clipped));
			}
			else {
				module->sequenceCapOverride = clipped;
				// Keep knob parked at Full when cap exceeds knob's native range.
				module->params[Crownstep::SEQ_LENGTH_PARAM].setValue(float(SEQ_LENGTH_MAX));
			}
		}
	}

	void applyClipCountForLocalX(float localX, int historySize, const RibbonLayout& layout) {
		applyClipCount(clipCountForLocalX(localX, historySize, layout));
	}

	int capValueFromModule() const {
		if (!module) {
			return 0;
		}
		int cap = module->currentSequenceCap();
		return std::max(0, cap);
	}

	int presetIndexForCapValue(int capValue) const {
		if (capValue <= 0) {
			return 0;
		}
		int bestIndex = 1;
		int bestDist = std::abs(PRESET_CAP_VALUES[1] - capValue);
		for (int i = 2; i < PRESET_COUNT; ++i) {
			int dist = std::abs(PRESET_CAP_VALUES[i] - capValue);
			if (dist < bestDist) {
				bestDist = dist;
				bestIndex = i;
			}
		}
		return bestIndex;
	}

	int currentPresetIndex() const {
		return presetIndexForCapValue(capValueFromModule());
	}

	RibbonState previewState() const {
		RibbonState s;
		s.historySize = 16;
		s.activeStart = 0;
		s.activeLength = 16;
		s.playbackIndex = 6;
		s.capValue = 16;
		s.fullMode = false;
		s.eventFlash = 0.f;
		s.stepWeight = {
			0.22f, 0.18f, 0.34f, 0.24f,
			0.42f, 0.20f, 0.30f, 0.26f,
			0.48f, 0.22f, 0.36f, 0.24f,
			0.54f, 0.28f, 0.40f, 0.32f
		};
		return s;
	}

	int presetIndexForLocalX(float x) const {
		float innerW = std::max(1.f, box.size.x - 4.f);
		float norm = clamp((x - 2.f) / innerW, 0.f, 1.f);
		int index = int(std::round(norm * float(PRESET_COUNT - 1)));
		return clamp(index, 0, PRESET_COUNT - 1);
	}

	void applyPresetIndex(int presetIndex) {
		if (!module) {
			return;
		}
		int idx = clamp(presetIndex, 0, PRESET_COUNT - 1);
		int capValue = PRESET_CAP_VALUES[idx];
		if (capValue > 0) {
			lastRecentCap = capValue;
			module->sequenceCapOverride = -1;
			module->params[Crownstep::SEQ_LENGTH_PARAM].setValue(float(capValue));
		}
		else {
			module->sequenceCapOverride = -1;
			module->params[Crownstep::SEQ_LENGTH_PARAM].setValue(float(SEQ_LENGTH_MAX));
		}
	}

	void nudgePreset(int dir) {
		int current = currentPresetIndex();
		int next = current;
		if (dir > 0) {
			next = std::min(current + 1, PRESET_COUNT - 1);
		}
		else if (dir < 0) {
			next = std::max(current - 1, 0);
		}
		applyPresetIndex(next);
	}

	void nudgeClipCount(int dir, int historySize) {
		if (!module || historySize <= 0 || dir == 0) {
			return;
		}
		int cap = capValueFromModule();
		if (cap <= 0) {
			// From FULL, decreasing moves to historySize-1; increasing stays FULL.
			if (dir < 0) {
				applyClipCount(std::max(1, historySize - 1));
			}
			else {
				applyClipCount(0);
			}
			return;
		}
		int next = cap + dir;
		if (next >= historySize) {
			applyClipCount(0); // Full
			return;
		}
		next = clamp(next, 1, std::max(1, historySize - 1));
		applyClipCount(next);
	}

	void toggleFullVsRecent() {
		int capValue = capValueFromModule();
		if (capValue <= 0) {
			applyPresetIndex(presetIndexForCapValue(lastRecentCap));
		}
		else {
			lastRecentCap = capValue;
			applyPresetIndex(0);
		}
	}

	RibbonState pullState() const {
		if (!module) {
			return previewState();
		}

		RibbonState s;
		s.activeLength = std::max(0, module->activeLength());
		s.activeStart = std::max(0, module->activeStartIndex());
		s.capValue = capValueFromModule();
		s.fullMode = (s.capValue == 0);
		if (s.activeLength > 0) {
			s.playbackIndex = clamp(module->displayedStep - 1, 0, s.activeLength - 1);
		}
		s.eventFlash = clamp(module->captureFlashSeconds / 0.16f, 0.f, 1.f);

		std::lock_guard<std::recursive_mutex> lock(module->sequenceMutex);
		s.historySize = int(module->moveHistory.size());
		s.stepWeight.assign(size_t(s.activeLength), 0.15f);
		for (int i = 0; i < s.activeLength; ++i) {
			int absIndex = s.activeStart + i;
			float w = 0.18f;
			if (absIndex >= 0 && absIndex < int(module->history.size())) {
				const Step& step = module->history[size_t(absIndex)];
				w += clamp(step.accent * 0.14f, 0.f, 0.42f);
			}
			if (absIndex >= 0 && absIndex < int(module->moveHistory.size())) {
				const Move& move = module->moveHistory[size_t(absIndex)];
				if (move.isCapture) {
					w += 0.16f;
				}
				if (move.isMultiCapture) {
					w += 0.14f;
				}
				if (move.isKing) {
					w += 0.18f;
				}
			}
			s.stepWeight[size_t(i)] = clamp(w, 0.10f, 1.f);
		}
		return s;
	}

	void onHover(const event::Hover& e) override {
		if (!module) {
			Widget::onHover(e);
			return;
		}
		lastHoverPos = e.pos;
		OpaqueWidget::onHover(e);
	}

	void onLeave(const event::Leave& e) override {
		if (!module) {
			Widget::onLeave(e);
			return;
		}
		OpaqueWidget::onLeave(e);
	}

	void onHoverScroll(const event::HoverScroll& e) override {
		if (!module) {
			Widget::onHoverScroll(e);
			return;
		}
		int mods = (APP && APP->window) ? APP->window->getMods() : 0;
		if ((mods & RACK_MOD_CTRL) != 0) {
			Widget::onHoverScroll(e);
			return;
		}
		float scroll = -e.scrollDelta.y;
		if (std::fabs(scroll) < 1e-4f) {
			Widget::onHoverScroll(e);
			return;
		}
		RibbonState s = pullState();
		int stepDir = (scroll > 0.f) ? 1 : -1;
		// One sequence-step per wheel event, regardless of scroll magnitude.
		nudgeClipCount(stepDir, s.historySize);
		e.consume(this);
	}

	void onButton(const event::Button& e) override {
		if (!module) {
			Widget::onButton(e);
			return;
		}
		if (e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			Widget::onButton(e);
			return;
		}
		lastHoverPos = e.pos;
		capDragLocal = e.pos;
		RibbonLayout layout = computeLayout();
		RibbonState s = pullState();
		if (pointInHistoryStrip(e.pos, layout)) {
			applyClipCountForLocalX(e.pos.x, s.historySize, layout);
			e.consume(this);
			return;
		}
		if (pointInLoopMinusButton(e.pos, layout)) {
			nudgeClipCount(-1, s.historySize);
			e.consume(this);
			return;
		}
		if (pointInLoopPlusButton(e.pos, layout)) {
			nudgeClipCount(1, s.historySize);
			e.consume(this);
			return;
		}
		Widget::onButton(e);
		return;
	}

	void onDragStart(const event::DragStart& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			Widget::onDragStart(e);
			return;
		}
		capDragActive = true;
		capDragLocal = currentLocalMousePos();
		lastHoverPos = capDragLocal;
		RibbonLayout layout = computeLayout();
		RibbonState s = pullState();
		capDragTrimMode = pointInHistoryStrip(capDragLocal, layout);
		if (capDragTrimMode) {
			applyClipCountForLocalX(capDragLocal.x, s.historySize, layout);
		}
		else {
			capDragActive = false;
			e.consume(this);
			return;
		}
		e.consume(this);
	}

	void onDragMove(const event::DragMove& e) override {
		if (!capDragActive || !module || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			Widget::onDragMove(e);
			return;
		}
		capDragLocal = currentLocalMousePos();
		capDragLocal.x = clamp(capDragLocal.x, 0.f, box.size.x);
		capDragLocal.y = clamp(capDragLocal.y, 0.f, box.size.y);
		lastHoverPos = capDragLocal;
		RibbonLayout layout = computeLayout();
		RibbonState s = pullState();
		if (capDragTrimMode) {
			applyClipCountForLocalX(capDragLocal.x, s.historySize, layout);
		}
		e.consume(this);
	}

	void onDragEnd(const event::DragEnd& e) override {
		if (!capDragActive || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			Widget::onDragEnd(e);
			return;
		}
		capDragActive = false;
		capDragTrimMode = false;
		e.consume(this);
	}

	void onDoubleClick(const event::DoubleClick& e) override {
		if (!module) {
			Widget::onDoubleClick(e);
			return;
		}
		RibbonLayout layout = computeLayout();
		Vec localPos = currentLocalMousePos();
		if (!pointInHistoryStrip(localPos, layout)) {
			e.consume(this);
			return;
		}
		toggleFullVsRecent();
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		RibbonState s = pullState();
		int currentStep = (s.activeLength > 0 && s.playbackIndex >= 0) ? (s.playbackIndex + 1) : 0;
		int totalSteps = s.activeLength;

			const float x = 0.f;
			const float y = 0.f;
			const float w = box.size.x;
			const float h = box.size.y;

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x, y, w, h, 4.f);
		nvgFillColor(args.vg, nvgRGBA(9, 11, 14, 194));
		nvgFill(args.vg);

				RibbonLayout layout = computeLayout();
				const bool compactLayout = layout.compact;
				float stripX = x + layout.stripX;
				float stripW = layout.stripW;
				float historyY = y + layout.historyY;
				float historyH = layout.historyH;
				float loopY = y + layout.loopY;
				float loopH = layout.loopH;
				float loopButtonW = loopButtonWidth(layout);
				float loopCenterX = loopDataX(layout);
				float loopCenterW = loopDataW(layout);
				Vec drawMouseLocal = currentLocalMousePos();
				bool hoverMinus = pointInLoopMinusButton(drawMouseLocal, layout);
				bool hoverPlus = pointInLoopPlusButton(drawMouseLocal, layout);

		// Full-history base strip.
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, stripX, historyY, stripW, historyH, 1.6f);
			nvgFillColor(args.vg, nvgRGBA(32, 40, 46, 146));
			nvgFill(args.vg);
				if (s.historySize > 0) {
					float startNorm = 0.f;
					float endNorm = 1.f;
				if (!s.fullMode && s.historySize > 0) {
					startNorm = clamp(float(s.activeStart) / float(std::max(1, s.historySize)), 0.f, 1.f);
					endNorm = clamp(float(s.activeStart + s.activeLength) / float(std::max(1, s.historySize)), 0.f, 1.f);
				}
				float activeX = stripX + stripW * startNorm;
				float activeW = std::max(1.4f, stripW * std::max(0.f, endNorm - startNorm));
				auto makeBrandActivePaint = [&](int alphaA, int alphaB) {
					return nvgLinearGradient(
						args.vg,
						activeX,
						historyY,
						activeX + activeW,
						historyY,
						nvgRGBA(85, 64, 178, alphaA),  // darkened #7a5cff
						nvgRGBA(20, 143, 152, alphaB)  // darkened #1cccd9
					);
				};
				if (s.fullMode) {
					nvgBeginPath(args.vg);
					nvgRoundedRect(args.vg, activeX, historyY + 0.2f, activeW, std::max(1.f, historyH - 0.4f), 1.2f);
					NVGpaint fullPaint = makeBrandActivePaint(238, 228);
					nvgFillPaint(args.vg, fullPaint);
					nvgFill(args.vg);
					nvgBeginPath(args.vg);
					nvgRoundedRect(args.vg, activeX, historyY + 0.2f, activeW, std::max(1.f, historyH - 0.4f), 1.2f);
					nvgStrokeColor(args.vg, nvgRGBA(255, 214, 122, 190));
					nvgStrokeWidth(args.vg, 0.85f);
					nvgStroke(args.vg);
				}
			else {
				// Dim non-active history more aggressively so RECENT window reads clearly.
				if (activeX > stripX + 0.5f) {
					nvgBeginPath(args.vg);
					nvgRoundedRect(args.vg, stripX, historyY + 0.3f, activeX - stripX, std::max(1.f, historyH - 0.6f), 1.1f);
					nvgFillColor(args.vg, nvgRGBA(12, 16, 20, 168));
					nvgFill(args.vg);
				}
				float rightX = activeX + activeW;
				float rightW = (stripX + stripW) - rightX;
				if (rightW > 0.5f) {
					nvgBeginPath(args.vg);
					nvgRoundedRect(args.vg, rightX, historyY + 0.3f, rightW, std::max(1.f, historyH - 0.6f), 1.1f);
					nvgFillColor(args.vg, nvgRGBA(12, 16, 20, 168));
					nvgFill(args.vg);
					}
					nvgBeginPath(args.vg);
					nvgRoundedRect(args.vg, activeX, historyY + 0.2f, activeW, std::max(1.f, historyH - 0.4f), 1.2f);
					NVGpaint recentPaint = makeBrandActivePaint(238, 228);
					nvgFillPaint(args.vg, recentPaint);
					nvgFill(args.vg);
					nvgBeginPath(args.vg);
				nvgRoundedRect(args.vg, activeX, historyY + 0.2f, activeW, std::max(1.f, historyH - 0.4f), 1.2f);
				nvgStrokeColor(args.vg, nvgRGBA(202, 236, 255, 198));
				nvgStrokeWidth(args.vg, 0.85f);
				nvgStroke(args.vg);
			}
		}

		// Loop detail strip.
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, stripX, loopY, stripW, loopH, 1.4f);
		nvgFillColor(args.vg, nvgRGBA(18, 24, 30, 188));
		nvgFill(args.vg);

		auto drawLoopButton = [&](float bx, const char* label, bool hovered) {
			float by = loopY + 0.25f;
			float bh = std::max(1.f, loopH - 0.5f);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, bx, by, loopButtonW, bh, 1.1f);
			nvgFillColor(args.vg, hovered ? nvgRGBA(34, 46, 58, 224) : nvgRGBA(25, 34, 44, 210));
			nvgFill(args.vg);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, bx, by, loopButtonW, bh, 1.1f);
			nvgStrokeColor(args.vg, hovered ? nvgRGBA(176, 224, 255, 196) : nvgRGBA(108, 140, 168, 142));
			nvgStrokeWidth(args.vg, 0.8f);
			nvgStroke(args.vg);
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			float glyphSize = compactLayout ? 7.0f : 8.3f;
			if (label && label[0] == '-' && label[1] == '\0') {
				glyphSize += compactLayout ? 1.0f : 1.35f;
			}
			nvgFontSize(args.vg, glyphSize);
			nvgFillColor(args.vg, hovered ? nvgRGBA(238, 248, 255, 244) : nvgRGBA(198, 216, 232, 222));
			nvgText(args.vg, bx + loopButtonW * 0.5f, loopY + loopH * 0.53f, label, nullptr);
		};
		drawLoopButton(stripX, "-", hoverMinus);
		drawLoopButton(stripX + stripW - loopButtonW, "+", hoverPlus);
		auto drawActiveLoopMarker = [&](float hx, float hy, float hw, float hh, float radius) {
			float shadowGrow = compactLayout ? 0.22f : 0.30f;
			float innerInset = compactLayout ? 0.18f : 0.24f;
			float hotInsetX = compactLayout ? 0.24f : 0.32f;
			float hotInsetY = compactLayout ? 0.10f : 0.14f;
			float hotH = std::max(0.24f, hh * 0.34f);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, hx - shadowGrow, hy - shadowGrow, hw + shadowGrow * 2.f, hh + shadowGrow * 2.f, radius + shadowGrow);
			nvgFillColor(args.vg, nvgRGBA(10, 14, 20, 206));
			nvgFill(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, hx, hy, hw, hh, radius);
			NVGpaint activePaint = nvgLinearGradient(
				args.vg,
				hx,
				hy,
				hx,
				hy + hh,
				nvgRGBA(255, 236, 148, 238),
				nvgRGBA(236, 160, 48, 230)
			);
			nvgFillPaint(args.vg, activePaint);
			nvgFill(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(
				args.vg,
				hx + hotInsetX,
				hy + hotInsetY,
				std::max(0.30f, hw - hotInsetX * 2.f),
				hotH,
				std::max(0.16f, radius * 0.62f)
			);
			nvgFillColor(args.vg, nvgRGBA(255, 247, 220, 176));
			nvgFill(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(
				args.vg,
				hx + innerInset,
				hy + innerInset,
				std::max(0.34f, hw - innerInset * 2.f),
				std::max(0.34f, hh - innerInset * 2.f),
				std::max(0.18f, radius * 0.78f)
			);
			nvgStrokeColor(args.vg, nvgRGBA(255, 246, 214, 248));
			nvgStrokeWidth(args.vg, compactLayout ? 0.7f : 0.9f);
			nvgStroke(args.vg);
		};
		auto drawChunkLoopMarker = [&](float hx, float hy, float hw, float hh, float radius) {
			float shadowGrow = compactLayout ? 0.22f : 0.30f;
			float innerInset = compactLayout ? 0.18f : 0.24f;
			float hotInsetX = compactLayout ? 0.24f : 0.32f;
			float hotInsetY = compactLayout ? 0.10f : 0.14f;
			float hotH = std::max(0.24f, hh * 0.34f);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, hx - shadowGrow, hy - shadowGrow, hw + shadowGrow * 2.f, hh + shadowGrow * 2.f, radius + shadowGrow);
			nvgFillColor(args.vg, nvgRGBA(8, 18, 24, 210));
			nvgFill(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, hx, hy, hw, hh, radius);
			NVGpaint activePaint = nvgLinearGradient(
				args.vg,
				hx,
				hy,
				hx,
				hy + hh,
				nvgRGBA(110, 232, 255, 236),
				nvgRGBA(26, 178, 214, 228)
			);
			nvgFillPaint(args.vg, activePaint);
			nvgFill(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(
				args.vg,
				hx + hotInsetX,
				hy + hotInsetY,
				std::max(0.30f, hw - hotInsetX * 2.f),
				hotH,
				std::max(0.16f, radius * 0.62f)
			);
			nvgFillColor(args.vg, nvgRGBA(220, 250, 255, 170));
			nvgFill(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(
				args.vg,
				hx + innerInset,
				hy + innerInset,
				std::max(0.34f, hw - innerInset * 2.f),
				std::max(0.34f, hh - innerInset * 2.f),
				std::max(0.18f, radius * 0.78f)
			);
			nvgStrokeColor(args.vg, nvgRGBA(210, 248, 255, 244));
			nvgStrokeWidth(args.vg, compactLayout ? 0.7f : 0.9f);
			nvgStroke(args.vg);
		};

			if (s.activeLength > 0) {
				VisualMode mode = chooseMode(s.activeLength);
				if (mode == VisualMode::DISCRETE) {
					float gap = 1.2f;
					float cellW = (loopCenterW - gap * float(s.activeLength - 1)) / float(s.activeLength);
					cellW = std::max(1.f, cellW);
					for (int i = 0; i < s.activeLength; ++i) {
						float cx = loopCenterX + float(i) * (cellW + gap);
						nvgBeginPath(args.vg);
						nvgRoundedRect(args.vg, cx, loopY + 0.35f, cellW, std::max(0.8f, loopH - 0.7f), 1.f);
						nvgFillColor(args.vg, nvgRGBA(112, 152, 184, 178));
						nvgFill(args.vg);
						if (i == s.playbackIndex) {
							drawActiveLoopMarker(cx - 0.2f, loopY + 0.2f, cellW + 0.4f, std::max(1.f, loopH - 0.4f), 1.f);
						}
					}
				}
				else if (mode == VisualMode::DOUBLE_ROW) {
					const int colsPerRow = 64;
					float colGap = 1.2f;
					float rowGap = compactLayout ? 0.55f : 0.8f;
					float cellW = (loopCenterW - colGap * float(colsPerRow - 1)) / float(colsPerRow);
					cellW = std::max(1.f, cellW);
					float cellH = (loopH - rowGap - 0.7f) * 0.5f;
					cellH = std::max(0.7f, cellH);
					for (int i = 0; i < s.activeLength; ++i) {
						int row = i / colsPerRow;
						int col = i % colsPerRow;
						float cx = loopCenterX + float(col) * (cellW + colGap);
						float cy = loopY + 0.35f + float(row) * (cellH + rowGap);
						nvgBeginPath(args.vg);
						nvgRoundedRect(args.vg, cx, cy, cellW, cellH, 0.7f);
						nvgFillColor(args.vg, nvgRGBA(112, 152, 184, 178));
						nvgFill(args.vg);
						if (i == s.playbackIndex) {
							drawActiveLoopMarker(cx - 0.2f, cy - 0.15f, cellW + 0.4f, cellH + 0.3f, 0.8f);
						}
					}
				}
				else if (mode == VisualMode::CHUNKED_64) {
					const int chunkSize = 64;
					int chunkCount = std::max(1, (s.activeLength + chunkSize - 1) / chunkSize);
					int chunkIndex = clamp(s.playbackIndex / chunkSize, 0, std::max(0, chunkCount - 1));
					int localIndex = clamp(s.playbackIndex - chunkIndex * chunkSize, 0, chunkSize - 1);
					int chunkLength = std::min(chunkSize, std::max(0, s.activeLength - chunkIndex * chunkSize));

					float rowGap = compactLayout ? 0.55f : 0.8f;
					float cellH = (loopH - rowGap - 0.7f) * 0.5f;
					cellH = std::max(0.7f, cellH);

					// Top row: fixed 64-slot local view for the current chunk.
					{
						const int cols = 64;
						float colGap = 1.2f;
						float cellW = (loopCenterW - colGap * float(cols - 1)) / float(cols);
						cellW = std::max(1.f, cellW);
						float cy = loopY + 0.35f;
						for (int i = 0; i < cols; ++i) {
							float cx = loopCenterX + float(i) * (cellW + colGap);
							nvgBeginPath(args.vg);
							nvgRoundedRect(args.vg, cx, cy, cellW, cellH, 0.7f);
							int alpha = (i < chunkLength) ? 178 : 68;
							nvgFillColor(args.vg, nvgRGBA(112, 152, 184, alpha));
							nvgFill(args.vg);
							if (i == localIndex) {
								drawActiveLoopMarker(cx - 0.2f, cy - 0.15f, cellW + 0.4f, cellH + 0.3f, 0.8f);
							}
						}
					}

					// Bottom row: progress through total chunk count (activeLength / 64).
					{
						float colGap = compactLayout ? 0.65f : 0.85f;
						float cellW = (loopCenterW - colGap * float(std::max(0, chunkCount - 1))) / float(std::max(1, chunkCount));
						cellW = std::max(1.f, cellW);
						float cy = loopY + 0.35f + cellH + rowGap;
						for (int i = 0; i < chunkCount; ++i) {
							float cx = loopCenterX + float(i) * (cellW + colGap);
							nvgBeginPath(args.vg);
							nvgRoundedRect(args.vg, cx, cy, cellW, cellH, 0.7f);
							nvgFillColor(args.vg, nvgRGBA(112, 152, 184, 178));
							nvgFill(args.vg);
							if (i == chunkIndex) {
								drawChunkLoopMarker(cx - 0.2f, cy - 0.15f, cellW + 0.4f, cellH + 0.3f, 0.8f);
							}
						}
					}
				}
				else if (mode == VisualMode::COMPRESSED) {
					int segCount = std::max(1, 64);
					float gap = 0.55f;
					float segW = (loopCenterW - gap * float(segCount - 1)) / float(segCount);
					segW = std::max(1.f, segW);
					for (int seg = 0; seg < segCount; ++seg) {
						int begin = int(std::floor(float(seg) * float(s.activeLength) / float(segCount)));
						int end = int(std::floor(float(seg + 1) * float(s.activeLength) / float(segCount)));
					end = std::max(end, begin + 1);
					end = std::min(end, s.activeLength);
						if (begin >= end) {
							continue;
						}
						float sx = loopCenterX + float(seg) * (segW + gap);
						nvgBeginPath(args.vg);
						nvgRoundedRect(args.vg, sx, loopY + 0.35f, segW, std::max(0.8f, loopH - 0.7f), 0.8f);
						nvgFillColor(args.vg, nvgRGBA(112, 152, 184, 178));
						nvgFill(args.vg);
						if (s.playbackIndex >= begin && s.playbackIndex < end) {
							drawActiveLoopMarker(sx, loopY + 0.2f, std::max(1.f, segW), std::max(1.f, loopH - 0.4f), 0.6f);
						}
					}
				}
			}

				// Current marker in history strip.
				if (s.historySize > 0 && s.activeLength > 0 && s.playbackIndex >= 0) {
				// Tie crown/line marker to local playback position within the
				// currently highlighted (purple) active window.
				float startNorm = 0.f;
				float endNorm = 1.f;
				if (!s.fullMode && s.historySize > 0) {
					startNorm = clamp(float(s.activeStart) / float(std::max(1, s.historySize)), 0.f, 1.f);
					endNorm = clamp(float(s.activeStart + s.activeLength) / float(std::max(1, s.historySize)), 0.f, 1.f);
				}
				float activeX = stripX + stripW * startNorm;
				float activeW = std::max(1.4f, stripW * std::max(0.f, endNorm - startNorm));
				float localNorm = (s.activeLength <= 1) ? 0.f : (float(s.playbackIndex) / float(s.activeLength - 1));
				float mx = activeX + activeW * clamp(localNorm, 0.f, 1.f);
				float markerCy = historyY - 0.1f;
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, mx, historyY - 0.4f);
				nvgLineTo(args.vg, mx, historyY + historyH + 0.4f);
			nvgStrokeColor(args.vg, nvgRGBA(255, 232, 176, 250));
			nvgStrokeWidth(args.vg, 1.15f);
			nvgStroke(args.vg);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, mx, markerCy, compactLayout ? 1.7f : 1.9f);
			NVGpaint markerPaint = nvgLinearGradient(
				args.vg,
				mx,
				markerCy - 1.8f,
				mx,
				markerCy + 1.8f,
				nvgRGBA(255, 237, 172, 250),
				nvgRGBA(240, 182, 78, 248)
			);
			nvgFillPaint(args.vg, markerPaint);
			nvgFill(args.vg);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, mx, markerCy, compactLayout ? 1.7f : 1.9f);
			nvgStrokeColor(args.vg, nvgRGBA(146, 104, 42, 216));
			nvgStrokeWidth(args.vg, 0.55f);
			nvgStroke(args.vg);

			if (!compactLayout) {
				// Integrated crown rising out of the round playhead badge.
				float crownW = 5.8f;
				float crownH = 4.0f;
				float crownBaseY = markerCy - 0.35f;
				float crownTop = crownBaseY - crownH;
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, mx - crownW * 0.42f, crownBaseY);
				nvgLineTo(args.vg, mx - crownW * 0.34f, crownTop + crownH * 0.34f);
				nvgLineTo(args.vg, mx - crownW * 0.16f, crownBaseY - crownH * 0.30f);
				nvgLineTo(args.vg, mx, crownTop);
				nvgLineTo(args.vg, mx + crownW * 0.16f, crownBaseY - crownH * 0.30f);
				nvgLineTo(args.vg, mx + crownW * 0.34f, crownTop + crownH * 0.34f);
				nvgLineTo(args.vg, mx + crownW * 0.42f, crownBaseY);
				nvgLineTo(args.vg, mx + crownW * 0.28f, crownBaseY + crownH * 0.14f);
				nvgLineTo(args.vg, mx - crownW * 0.28f, crownBaseY + crownH * 0.14f);
				nvgClosePath(args.vg);
				NVGpaint crownPaint = nvgLinearGradient(
					args.vg,
					mx,
					crownTop,
					mx,
					crownBaseY + crownH * 0.16f,
					nvgRGBA(255, 236, 164, 248),
					nvgRGBA(238, 186, 74, 246)
				);
				nvgFillPaint(args.vg, crownPaint);
				nvgFill(args.vg);
				nvgStrokeColor(args.vg, nvgRGBA(146, 104, 42, 220));
				nvgStrokeWidth(args.vg, 0.55f);
				nvgStroke(args.vg);
				}
			}

			// Centered status text inside the top (history) strip.
			char ribbonText[40];
			std::snprintf(ribbonText, sizeof(ribbonText), "%d / %d", currentStep, totalSteps);
			char fullText[24];
			std::snprintf(fullText, sizeof(fullText), "%d", s.historySize);
			float fullX = stripX + (compactLayout ? 3.1f : 4.0f);
			float textY = historyY + historyH * 0.5f;
			float fullMaxW = std::max(10.f, stripW * (compactLayout ? 0.18f : 0.16f));
			float textBounds[4];
			float sharedFontSize = compactLayout ? 7.2f : 8.4f;
			float fullMeasuredW = 0.f;
			float reservedLeftW = 0.f;
			for (int i = 0; i < 9; ++i) {
				nvgFontSize(args.vg, sharedFontSize);
				nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
				nvgTextBounds(args.vg, 0.f, 0.f, fullText, nullptr, textBounds);
				fullMeasuredW = textBounds[2] - textBounds[0];
				reservedLeftW = std::max(fullMaxW, fullMeasuredW) + (compactLayout ? 4.1f : 5.0f);
				float centerMaxW = std::max(14.f, stripW - reservedLeftW - (compactLayout ? 4.5f : 5.5f));
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
				nvgTextBounds(args.vg, 0.f, 0.f, ribbonText, nullptr, textBounds);
				float ribbonW = textBounds[2] - textBounds[0];
				if ((fullMeasuredW <= fullMaxW && ribbonW <= centerMaxW && sharedFontSize <= historyH * 0.95f + 0.1f) || sharedFontSize <= 5.6f) {
					break;
				}
				sharedFontSize -= 0.3f;
			}

			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			float textX = stripX + stripW * 0.5f;
			nvgFontSize(args.vg, sharedFontSize);
			float shadowDx = compactLayout ? 0.42f : 0.50f;
			float shadowDy = compactLayout ? 0.48f : 0.56f;
			nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 156));
			nvgText(args.vg, textX + shadowDx, textY + shadowDy, ribbonText, nullptr);
			nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 242));
			nvgText(args.vg, textX, textY, ribbonText, nullptr);

			if (s.historySize > 0) {
				nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
				nvgFontSize(args.vg, sharedFontSize);
				nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 156));
				nvgText(args.vg, fullX + shadowDx, textY + shadowDy, fullText, nullptr);
				nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 236));
				nvgText(args.vg, fullX, textY, fullText, nullptr);
			}

				// Hover preview tooltip: only show while the mouse is actively over
				// the ribbon, or while an active trim drag is in progress.
				Vec hoverLocal = currentLocalMousePos();
				bool hoverInRibbon = pointInHistoryStrip(hoverLocal, layout) || pointInLoopStrip(hoverLocal, layout);
				Vec tooltipLocal = capDragActive ? capDragLocal : hoverLocal;
				bool previewClip = (s.historySize > 0)
					&& ((capDragActive && capDragTrimMode) || pointInHistoryStrip(tooltipLocal, layout));
				if (previewClip) {
					int clipCount = clipCountForLocalX(tooltipLocal.x, s.historySize, layout);
					char clipText[48];
					formatWindowPreviewText(clipCount, s.historySize, clipText, sizeof(clipText));
					float fontSize = compactLayout ? 6.2f : 7.0f;
					float textBounds[4];
					nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
					nvgFontSize(args.vg, fontSize);
					nvgTextBounds(args.vg, 0.f, 0.f, clipText, nullptr, textBounds);
					float textW = textBounds[2] - textBounds[0];
					float textH = textBounds[3] - textBounds[1];
					float padX = 4.f;
					float padY = 2.8f;
					float bubbleW = textW + padX * 2.f;
					float bubbleH = textH + padY * 2.f;
					float bx = clamp(tooltipLocal.x + 10.f, 1.f, std::max(1.f, w - bubbleW - 1.f));
					float by = clamp(tooltipLocal.y - bubbleH - 5.f, 1.f, std::max(1.f, h - bubbleH - 1.f));

					nvgBeginPath(args.vg);
					nvgRoundedRect(args.vg, bx, by, bubbleW, bubbleH, 2.6f);
					nvgFillColor(args.vg, nvgRGBA(14, 18, 24, 228));
					nvgFill(args.vg);
					nvgBeginPath(args.vg);
					nvgRoundedRect(args.vg, bx, by, bubbleW, bubbleH, 2.6f);
					nvgStrokeColor(args.vg, nvgRGBA(146, 198, 236, 158));
					nvgStrokeWidth(args.vg, 0.9f);
					nvgStroke(args.vg);
					nvgFillColor(args.vg, nvgRGBA(228, 244, 255, 240));
					nvgText(args.vg, bx + padX, by + padY, clipText, nullptr);
				}
				else if (s.historySize > 0 && hoverInRibbon
					&& (pointInLoopMinusButton(tooltipLocal, layout) || pointInLoopPlusButton(tooltipLocal, layout))) {
					int previewCount = pointInLoopMinusButton(tooltipLocal, layout)
						? nudgedClipCountPreview(-1, s.historySize)
						: nudgedClipCountPreview(1, s.historySize);
					char clipText[48];
					formatWindowPreviewText(previewCount, s.historySize, clipText, sizeof(clipText));
					float fontSize = compactLayout ? 6.2f : 7.0f;
					float textBounds[4];
					nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
					nvgFontSize(args.vg, fontSize);
					nvgTextBounds(args.vg, 0.f, 0.f, clipText, nullptr, textBounds);
					float textW = textBounds[2] - textBounds[0];
					float textH = textBounds[3] - textBounds[1];
					float padX = 4.f;
					float padY = 2.8f;
					float bubbleW = textW + padX * 2.f;
					float bubbleH = textH + padY * 2.f;
					float bx = clamp(tooltipLocal.x + 10.f, 1.f, std::max(1.f, w - bubbleW - 1.f));
					float by = clamp(tooltipLocal.y - bubbleH - 5.f, 1.f, std::max(1.f, h - bubbleH - 1.f));

					nvgBeginPath(args.vg);
					nvgRoundedRect(args.vg, bx, by, bubbleW, bubbleH, 2.6f);
					nvgFillColor(args.vg, nvgRGBA(14, 18, 24, 228));
					nvgFill(args.vg);
					nvgBeginPath(args.vg);
					nvgRoundedRect(args.vg, bx, by, bubbleW, bubbleH, 2.6f);
					nvgStrokeColor(args.vg, nvgRGBA(146, 198, 236, 158));
					nvgStrokeWidth(args.vg, 0.9f);
					nvgStroke(args.vg);
					nvgFillColor(args.vg, nvgRGBA(228, 244, 255, 240));
					nvgText(args.vg, bx + padX, by + padY, clipText, nullptr);
				}

		}
	};

constexpr int CrownRibbonWidget::PRESET_CAP_VALUES[CrownRibbonWidget::PRESET_COUNT];

struct CrownstepDifficultyItem final : MenuItem {
	Crownstep* module = nullptr;
	int difficulty = 0;

	void onAction(const event::Action& e) override {
		if (module) {
			module->aiDifficulty = difficulty;
		}
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->aiDifficulty == difficulty) ? "✓" : "";
		MenuItem::step();
	}
};

struct CrownstepAiThinkMsWidget final : TransparentWidget {
	Crownstep* module = nullptr;

	explicit CrownstepAiThinkMsWidget(Crownstep* crownstepModule) {
		module = crownstepModule;
	}

	void draw(const DrawArgs& args) override {
		if (!module || !args.vg) {
			return;
		}

		char text[64];
		std::snprintf(text, sizeof(text), "AI: %dms", std::max(0, module->lastAiThinkMs));

		nvgFontSize(args.vg, 10.0f);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGBA(16, 16, 16, 168));
		nvgText(args.vg, 0.45f, box.size.y * 0.52f + 0.45f, text, nullptr);
		nvgFillColor(args.vg, nvgRGBA(245, 221, 88, 236));
		nvgText(args.vg, 0.f, box.size.y * 0.52f, text, nullptr);
	}
};

struct CrownstepWidget final : ModuleWidget {
	explicit CrownstepWidget(Crownstep* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/crownstep.svg");
		try {
			setPanel(createPanel(panelPath));
		}
		catch (const std::exception& e) {
			WARN("Crownstep panel load failed (%s), using fallback: %s", panelPath.c_str(), e.what());
			setPanel(createPanel(asset::plugin(pluginInstance, "res/proc.svg")));
		}

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

			CrownstepBoardWidget* boardWidget = new CrownstepBoardWidget(module);
			math::Rect boardRectMm;
			if (panel_svg::loadRectFromSvgMm(panelPath, "BOARD_AREA", &boardRectMm)) {
				boardWidget->box.pos = mm2px(boardRectMm.pos);
				boardWidget->box.size = mm2px(boardRectMm.size);
			}
			else {
				boardRectMm = math::Rect(Vec(5.5f, 11.f), Vec(80.5f, 80.5f));
				boardWidget->box.pos = mm2px(boardRectMm.pos);
				boardWidget->box.size = mm2px(boardRectMm.size);
			}
			addChild(boardWidget);

			// Crown ribbon: anchored by SVG rect (STEP_COUNTER).
			math::Rect stepCounterRectMm;
			if (!panel_svg::loadRectFromSvgMm(panelPath, "STEP_COUNTER", &stepCounterRectMm)) {
				// Fallback for older panels that used a point anchor.
				const float fallbackWidthMm = 32.0f;
				const float fallbackHeightMm = 7.2f;
				Vec stepCounterCenterMm(45.72f, 100.3f);
				panel_svg::loadPointFromSvgMm(panelPath, "STEP_COUNTER", &stepCounterCenterMm);
				stepCounterRectMm = math::Rect(
					Vec(
						stepCounterCenterMm.x - fallbackWidthMm * 0.5f,
						stepCounterCenterMm.y - fallbackHeightMm * 0.5f
					),
					Vec(fallbackWidthMm, fallbackHeightMm)
				);
			}

			CrownRibbonWidget* stepCounterWidget = new CrownRibbonWidget(module);
			stepCounterWidget->box.pos = mm2px(stepCounterRectMm.pos);
			stepCounterWidget->box.size = mm2px(stepCounterRectMm.size);
			addChild(stepCounterWidget);

		// Bottom control layout:
		// left cluster = inputs, center = knobs/button, right cluster = outputs.
		math::Rect inputsAreaMm;
		math::Rect outputsAreaMm;
		math::Rect controlsAreaMm;
		bool hasInputsArea = panel_svg::loadRectFromSvgMm(panelPath, "INPUTS_AREA", &inputsAreaMm);
		bool hasOutputsArea = panel_svg::loadRectFromSvgMm(panelPath, "OUTPUTS_AREA", &outputsAreaMm);
		bool hasControlsArea = panel_svg::loadRectFromSvgMm(panelPath, "CONTROLS_AREA", &controlsAreaMm);
		bool hasBottomAnchors = hasInputsArea || hasOutputsArea || hasControlsArea;

		auto pointInRect = [](const math::Rect& rect, float u, float v) {
			return Vec(rect.pos.x + rect.size.x * u, rect.pos.y + rect.size.y * v);
		};

			Vec newGamePos(43.f, 114.0f);
		Vec debugAddMovesPos(5.08f, 5.08f);
		Vec aiThinkMsPos(debugAddMovesPos.x + 4.2f, debugAddMovesPos.y);
		Vec clockPos(12.f, 108.0f);
		Vec resetPos(28.f, 108.0f);
		Vec transposePos(12.f, 121.0f);
		Vec rootCvPos(28.f, 121.0f);
		Vec pitchPos(58.f, 108.0f);
		Vec accentPos(58.f, 121.0f);
		Vec modPos(74.f, 121.0f);
		Vec eocPos(86.f, 121.0f);
		Vec humanLightPos(82.5f, 93.f);
		Vec aiLightPos(86.5f, 93.f);

		if (hasInputsArea) {
			clockPos = pointInRect(inputsAreaMm, 0.30f, 0.38f);
			resetPos = pointInRect(inputsAreaMm, 0.70f, 0.38f);
			transposePos = pointInRect(inputsAreaMm, 0.30f, 0.78f);
			rootCvPos = pointInRect(inputsAreaMm, 0.70f, 0.78f);
		}
		if (hasOutputsArea) {
			pitchPos = pointInRect(outputsAreaMm, 0.17f, 0.58f);
			accentPos = pointInRect(outputsAreaMm, 0.39f, 0.58f);
			modPos = pointInRect(outputsAreaMm, 0.61f, 0.58f);
			eocPos = pointInRect(outputsAreaMm, 0.83f, 0.58f);
		}
		if (hasBottomAnchors) {
			float controlX = 43.f;
			float controlY = 98.f;
			float controlH = 22.f;
			if (hasControlsArea) {
				controlX = controlsAreaMm.pos.x + controlsAreaMm.size.x * 0.5f;
				controlY = controlsAreaMm.pos.y;
				controlH = controlsAreaMm.size.y;
			}
			else {
				if (hasInputsArea && hasOutputsArea) {
					float leftX = inputsAreaMm.pos.x + inputsAreaMm.size.x;
					float rightX = outputsAreaMm.pos.x;
					controlX = (leftX + rightX) * 0.5f;
					controlY = (inputsAreaMm.pos.y + outputsAreaMm.pos.y) * 0.5f;
					controlH = (inputsAreaMm.size.y + outputsAreaMm.size.y) * 0.5f;
				}
				else if (hasInputsArea) {
					controlX = inputsAreaMm.pos.x + inputsAreaMm.size.x + 8.f;
					controlY = inputsAreaMm.pos.y;
					controlH = inputsAreaMm.size.y;
				}
				else if (hasOutputsArea) {
					controlX = outputsAreaMm.pos.x - 8.f;
					controlY = outputsAreaMm.pos.y;
					controlH = outputsAreaMm.size.y;
				}
			}
				newGamePos = Vec(controlX, controlY + controlH * 0.76f);
			}

		// Prefer explicit component anchors from the SVG "components" layer.
		auto applyPointOverride = [&](const char* elementId, Vec* outPos) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
				*outPos = pointMm;
			}
		};
		auto applyPointOverrideFallback = [&](const char* primaryId, const char* fallbackId, Vec* outPos) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelPath, primaryId, &pointMm)
				|| panel_svg::loadPointFromSvgMm(panelPath, fallbackId, &pointMm)) {
				*outPos = pointMm;
			}
		};
			applyPointOverride("NEW_GAME_PARAM", &newGamePos);
		applyPointOverride("CLOCK_INPUT", &clockPos);
		applyPointOverride("RESET_INPUT", &resetPos);
		applyPointOverride("TRANSPOSE_INPUT", &transposePos);
		applyPointOverrideFallback("BIAS_INPUT", "ROOT_INPUT", &rootCvPos);
		applyPointOverride("PITCH_OUTPUT", &pitchPos);
		applyPointOverride("ACCENT_OUTPUT", &accentPos);
		applyPointOverride("MOD_OUTPUT", &modPos);
		applyPointOverride("EOC_OUTPUT", &eocPos);
		applyPointOverride("HUMAN_TURN_LIGHT", &humanLightPos);
		applyPointOverride("AI_TURN_LIGHT", &aiLightPos);
		applyPointOverride("AI_THINK_MS", &aiThinkMsPos);

			// SEQ_LENGTH_PARAM is intentionally soft-deprecated from GUI.
			// Runtime sequence length is controlled by the ribbon widget trim interactions.
			addParam(createParamCentered<LEDButton>(mm2px(newGamePos), module, Crownstep::NEW_GAME_PARAM));
			if (isDragonKingDebugEnabled()) {
				addParam(createParamCentered<LEDButton>(mm2px(debugAddMovesPos), module, Crownstep::DEBUG_ADD_MOVES_PARAM));
				CrownstepAiThinkMsWidget* aiThinkMsWidget = new CrownstepAiThinkMsWidget(module);
				aiThinkMsWidget->box.pos = mm2px(Vec(aiThinkMsPos.x + 2.7f, aiThinkMsPos.y - 1.2f));
				aiThinkMsWidget->box.size = mm2px(Vec(17.0f, 4.0f));
				addChild(aiThinkMsWidget);
			}

		addInput(createInputCentered<PJ301MPort>(mm2px(clockPos), module, Crownstep::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(resetPos), module, Crownstep::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(transposePos), module, Crownstep::TRANSPOSE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(rootCvPos), module, Crownstep::ROOT_INPUT));

		addOutput(createOutputCentered<BananutBlack>(mm2px(pitchPos), module, Crownstep::PITCH_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(accentPos), module, Crownstep::ACCENT_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(modPos), module, Crownstep::MOD_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(eocPos), module, Crownstep::EOC_OUTPUT));

		addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(humanLightPos), module, Crownstep::HUMAN_TURN_LIGHT));
		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(aiLightPos), module, Crownstep::AI_TURN_LIGHT));
	}

	void step() override {
		ModuleWidget::step();
		Crownstep* crownstepModule = dynamic_cast<Crownstep*>(module);
		if (crownstepModule) {
			crownstepModule->serviceAiTurnFromUiThread();
		}
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		Crownstep* module = dynamic_cast<Crownstep*>(this->module);
		menu->addChild(new MenuSeparator());
		MenuLabel* gameLabel = new MenuLabel();
		gameLabel->text = "Game";
		menu->addChild(gameLabel);
		menu->addChild(createSubmenuItem("Mode", "", [=](Menu* gameMenu) {
			for (int i = 0; i < int(GAME_MODE_NAMES.size()); ++i) {
				gameMenu->addChild(createCheckMenuItem(
					GAME_MODE_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->gameMode == i;
					},
					[=]() {
						if (module) {
							module->setGameMode(i, true);
						}
					}
				));
			}
		}));
		menu->addChild(createSubmenuItem("Player", "", [=](Menu* playerMenu) {
			for (int i = 0; i < int(PLAYER_MODE_NAMES.size()); ++i) {
				playerMenu->addChild(createCheckMenuItem(
					PLAYER_MODE_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->playerMode == i;
					},
					[=]() {
						if (module) {
							module->cancelAiTurnWork();
							module->playerMode = i;
							module->startNewGame();
						}
					}
				));
			}
		}));
		menu->addChild(createSubmenuItem("AI Difficulty", "", [=](Menu* difficultyMenu) {
			for (int i = 0; i < int(DIFFICULTY_NAMES.size()); ++i) {
				CrownstepDifficultyItem* item = new CrownstepDifficultyItem();
				item->text = DIFFICULTY_NAMES[size_t(i)];
				item->module = module;
				item->difficulty = i;
				difficultyMenu->addChild(item);
			}
		}));
		menu->addChild(createSubmenuItem("Highlight", "", [=](Menu* highlightMenu) {
			for (int i = 0; i < int(HIGHLIGHT_MODE_NAMES.size()); ++i) {
				highlightMenu->addChild(createCheckMenuItem(
					HIGHLIGHT_MODE_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->highlightMode == i;
					},
					[=]() {
						if (module) {
							module->highlightMode = i;
						}
					}
				));
			}
		}));
		if (!module || !module->isOthelloMode()) {
			menu->addChild(createSubmenuItem("Board Texture", "", [=](Menu* textureMenu) {
				for (int i = 0; i < int(BOARD_TEXTURE_NAMES.size()); ++i) {
					textureMenu->addChild(createCheckMenuItem(
						BOARD_TEXTURE_NAMES[size_t(i)],
						"",
						[=]() {
							return module && module->boardTextureMode == i;
						},
						[=]() {
							if (module) {
								module->boardTextureMode = i;
							}
						}
					));
				}
			}));
		}
		menu->addChild(new MenuSeparator());
		MenuLabel* quantizerLabel = new MenuLabel();
		quantizerLabel->text = "Quantizer";
		menu->addChild(quantizerLabel);
		menu->addChild(createCheckMenuItem(
			"Enable Quantization",
			"",
			[=]() {
				return module && module->quantizationEnabled;
			},
			[=]() {
				if (module) {
					module->quantizationEnabled = !module->quantizationEnabled;
				}
			}
		));
		menu->addChild(createSubmenuItem("Scale", "", [=](Menu* scaleMenu) {
			for (int i = 0; i < int(SCALES.size()); ++i) {
				scaleMenu->addChild(createCheckMenuItem(
					SCALES[size_t(i)].name,
					"",
					[=]() {
						return module && clamp(int(std::round(module->params[Crownstep::SCALE_PARAM].getValue())), 0,
							int(SCALES.size()) - 1) == i;
					},
					[=]() {
						if (module) {
							module->params[Crownstep::SCALE_PARAM].setValue(float(i));
						}
					}
				));
			}
		}));
		menu->addChild(createSubmenuItem("Key", "", [=](Menu* keyMenu) {
			for (int i = 0; i < int(KEY_NAMES.size()); ++i) {
				keyMenu->addChild(createCheckMenuItem(
					KEY_NAMES[size_t(i)],
					"",
					[=]() {
						return module && clamp(int(std::round(module->params[Crownstep::ROOT_PARAM].getValue())), 0, 11) == i;
					},
					[=]() {
						if (module) {
							module->params[Crownstep::ROOT_PARAM].setValue(float(i));
						}
					}
				));
			}
		}));
		menu->addChild(new MenuSeparator());
		MenuLabel* pitchLabel = new MenuLabel();
		pitchLabel->text = "Pitch";
		menu->addChild(pitchLabel);
		menu->addChild(createCheckMenuItem(
			"Show Cell Pitch Values",
			"",
			[=]() {
				return module && module->showCellPitchOverlay;
			},
			[=]() {
				if (module) {
					module->showCellPitchOverlay = !module->showCellPitchOverlay;
				}
			}
		));
		menu->addChild(createCheckMenuItem(
			"Bipolar",
			"",
			[=]() {
				return module && module->pitchBipolarEnabled;
			},
			[=]() {
				if (module) {
					module->pitchBipolarEnabled = !module->pitchBipolarEnabled;
				}
			}
		));
		menu->addChild(createCheckMenuItem(
			"Melodic Bias",
			"",
			[=]() {
				return module && module->melodicBiasEnabled;
			},
			[=]() {
				if (module) {
					module->melodicBiasEnabled = !module->melodicBiasEnabled;
					module->refreshHeldPitchForCurrentStep();
				}
			}
		));
		menu->addChild(createSubmenuItem("Board Layout", "", [=](Menu* valueLayoutMenu) {
			MenuLabel* currentLayoutLabel = new MenuLabel();
			currentLayoutLabel->text = module
				? std::string("Current: ") + BOARD_VALUE_LAYOUT_NAMES[size_t(clamp(
					module->boardValueLayoutMode,
					0,
					int(BOARD_VALUE_LAYOUT_NAMES.size()) - 1
				))] + (module->boardValueLayoutInverted ? " (Inverted)" : "")
				: "Current: Center-Out";
			valueLayoutMenu->addChild(currentLayoutLabel);
			valueLayoutMenu->addChild(new MenuSeparator());
			valueLayoutMenu->addChild(createCheckMenuItem(
				"Inverted",
				"",
				[=]() {
					return module && module->boardValueLayoutInverted;
				},
				[=]() {
					if (module) {
						module->boardValueLayoutInverted = !module->boardValueLayoutInverted;
						module->refreshHeldPitchForCurrentStep();
					}
				}
			));
			valueLayoutMenu->addChild(new MenuSeparator());
			valueLayoutMenu->addChild(createCheckMenuItem(
				BOARD_VALUE_LAYOUT_NAMES[0],
				"",
				[=]() {
					return module && module->boardValueLayoutMode == 0;
				},
				[=]() {
					if (module) {
						module->boardValueLayoutMode = 0;
						module->refreshHeldPitchForCurrentStep();
					}
				}
			));
			valueLayoutMenu->addChild(createSubmenuItem("Linear", "", [=](Menu* linearMenu) {
				for (int i : {1, 2, 3}) {
					linearMenu->addChild(createCheckMenuItem(
						BOARD_VALUE_LAYOUT_NAMES[size_t(i)],
						"",
						[=]() {
							return module && module->boardValueLayoutMode == i;
						},
						[=]() {
							if (module) {
								module->boardValueLayoutMode = i;
								module->refreshHeldPitchForCurrentStep();
							}
						}
					));
				}
			}));
			valueLayoutMenu->addChild(createSubmenuItem("Serpentine", "", [=](Menu* serpentineMenu) {
				for (int i : {4, 5, 6}) {
					serpentineMenu->addChild(createCheckMenuItem(
						BOARD_VALUE_LAYOUT_NAMES[size_t(i)],
						"",
						[=]() {
							return module && module->boardValueLayoutMode == i;
						},
						[=]() {
							if (module) {
								module->boardValueLayoutMode = i;
								module->refreshHeldPitchForCurrentStep();
							}
						}
					));
				}
			}));
			valueLayoutMenu->addChild(createMenuItem(
				"Randomize",
				"",
				[=]() {
					if (module) {
						module->boardValueLayoutMode = crownstep::BOARD_VALUE_LAYOUT_RANDOM;
						module->randomizeBoardValueLayout();
					}
				}
			));
		}));
		menu->addChild(createSubmenuItem("Source", "", [=](Menu* interpretationMenu) {
			for (int i = 0; i < int(PITCH_INTERPRETATION_NAMES.size()); ++i) {
				interpretationMenu->addChild(createCheckMenuItem(
					PITCH_INTERPRETATION_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->pitchInterpretationMode == i;
					},
					[=]() {
						if (module) {
							module->pitchInterpretationMode = i;
						}
					}
				));
			}
		}));
		menu->addChild(createSubmenuItem("Scalar", "", [=](Menu* dividerMenu) {
			dividerMenu->addChild(createCheckMenuItem(
				crownstep::PITCH_DIVIDER_NAMES[size_t(0)],
				"",
				[=]() {
					return module && module->pitchDividerMode == 0;
				},
				[=]() {
					if (module) {
						module->pitchDividerMode = 0;
						module->refreshHeldPitchForCurrentStep();
					}
				}
			));
			dividerMenu->addChild(new MenuSeparator());
			for (int mode = 1; mode < 4; ++mode) {
				dividerMenu->addChild(createCheckMenuItem(
					crownstep::PITCH_DIVIDER_NAMES[size_t(mode)],
					"",
					[=]() {
						return module && module->pitchDividerMode == mode;
					},
					[=]() {
						if (module) {
							module->pitchDividerMode = mode;
							module->refreshHeldPitchForCurrentStep();
						}
					}
				));
			}
			dividerMenu->addChild(new MenuSeparator());
			for (int mode = 4; mode < int(crownstep::PITCH_DIVIDER_NAMES.size()); ++mode) {
				dividerMenu->addChild(createCheckMenuItem(
					crownstep::PITCH_DIVIDER_NAMES[size_t(mode)],
					"",
					[=]() {
						return module && module->pitchDividerMode == mode;
					},
					[=]() {
						if (module) {
							module->pitchDividerMode = mode;
							module->refreshHeldPitchForCurrentStep();
						}
					}
				));
			}
		}));
	}
};

Model* modelCrownstep = createModel<Crownstep, CrownstepWidget>("Crownstep");
