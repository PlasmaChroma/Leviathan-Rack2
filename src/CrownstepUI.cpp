#include "CrownstepShared.hpp"
#include "PanelSvgUtils.hpp"

#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>

namespace {

constexpr int CHESS_ATLAS_ROWS = 2;
constexpr int CHESS_ATLAS_COLS = 6;
constexpr bool CHESS_ATLAS_ENABLED = true;
constexpr float CHESS_ATLAS_RASTER_SCALE = 2.f;
constexpr bool CHESS_ATLAS_SCALE_PER_COLOR = false;
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
	std::shared_ptr<Svg> svg;
	bool initialized = false;
	bool available = false;
	NVGcontext* rasterImageVg = nullptr;
	int rasterImageHandle = -1;
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
	if (cache->rasterImageHandle >= 0 && cache->rasterImageVg == vg) {
		return true;
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

	int imageHandle = nvgCreateImageRGBA(vg, rasterWidth, rasterHeight, NVG_IMAGE_GENERATE_MIPMAPS, pixels.data());
	if (imageHandle < 0) {
		return false;
	}
	cache->rasterImageVg = vg;
	cache->rasterImageHandle = imageHandle;
	cache->rasterImageWidth = rasterWidth;
	cache->rasterImageHeight = rasterHeight;
	cache->rasterScale = CHESS_ATLAS_RASTER_SCALE;
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
	if (scaleRefHeight <= 0.f) {
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

	nvgSave(vg);
	nvgBeginPath(vg);
	nvgRect(vg, x, y, drawWidth, drawHeight);
	NVGpaint imagePaint = nvgImagePattern(
		vg,
		patternX,
		patternY,
		atlasDrawWidth,
		atlasDrawHeight,
		0.f,
		cache.rasterImageHandle,
		clamp(alpha, 0.f, 1.f)
	);
	nvgFillPaint(vg, imagePaint);
	nvgFill(vg);
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
		bool othelloBoard = module && module->isOthelloMode();
		for (int row = 0; row < 8; ++row) {
			for (int col = 0; col < 8; ++col) {
				bool dark = ((row + col) & 1) == 1;
				bool marbleTexture = module && module->boardTextureMode == Crownstep::BOARD_TEXTURE_MARBLE;
				bool fabricTexture = module && module->boardTextureMode == Crownstep::BOARD_TEXTURE_FABRIC;
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
				else if (marbleTexture) {
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
				else if (fabricTexture) {
					NVGcolor topColor = dark ? nvgRGB(34, 118, 54) : nvgRGB(236, 244, 236);
					NVGcolor bottomColor = dark ? nvgRGB(18, 82, 38) : nvgRGB(206, 220, 206);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
					nvgFillPaint(args.vg, basePaint);
					nvgFill(args.vg);

					// Subtle cloth weave to keep the fabric board readable without heavy draw cost.
					NVGcolor weaveA = dark ? nvgRGBA(255, 255, 255, 14) : nvgRGBA(26, 48, 26, 14);
					NVGcolor weaveB = dark ? nvgRGBA(8, 26, 10, 18) : nvgRGBA(255, 255, 255, 20);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint weavePaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y + cellHeight, weaveA, weaveB);
					nvgFillPaint(args.vg, weavePaint);
					nvgFill(args.vg);

					for (int stripe = 0; stripe < 2; ++stripe) {
						float t = (stripe + 1.f) / 3.f;
						float vx = x + t * cellWidth;
						float hy = y + t * cellHeight;
						NVGcolor stripeColor = dark ? nvgRGBA(224, 255, 224, 22) : nvgRGBA(16, 38, 16, 18);

						nvgBeginPath(args.vg);
						nvgMoveTo(args.vg, vx, y + 0.7f);
						nvgLineTo(args.vg, vx, y + cellHeight - 0.7f);
						nvgStrokeColor(args.vg, stripeColor);
						nvgStrokeWidth(args.vg, 0.55f);
						nvgStroke(args.vg);

						nvgBeginPath(args.vg);
						nvgMoveTo(args.vg, x + 0.7f, hy);
						nvgLineTo(args.vg, x + cellWidth - 0.7f, hy);
						nvgStrokeColor(args.vg, stripeColor);
						nvgStrokeWidth(args.vg, 0.55f);
						nvgStroke(args.vg);
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

				if (module) {
					// Temporary UX tweak: hide AI potential-move hint dots/rings.
					const bool renderOpponentMoveHints = false;
					if (!module->gameOver && module->selectedSquare >= 0) {
					int row = 0;
					int col = 0;
					if (module->boardIndexToCoord(module->selectedSquare, &row, &col)) {
						float pulse = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.6f + 0.8f);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth - 1.f, row * cellHeight - 1.f, cellWidth + 2.f, cellHeight + 2.f);
						nvgFillColor(args.vg, nvgRGBA(88, 240, 154, int(24.f + 48.f * pulse)));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth, row * cellHeight, cellWidth, cellHeight);
						nvgStrokeColor(args.vg, nvgRGBA(98, 235, 154, int(188.f + 54.f * pulse)));
						nvgStrokeWidth(args.vg, 2.15f);
						nvgStroke(args.vg);
					}
				}
				if (!module->gameOver) {
					for (int destinationIndex : module->highlightedDestinations) {
						int row = 0;
						int col = 0;
						if (!module->boardIndexToCoord(destinationIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.43f;
						float breath = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.4f + phase);
						float glowRadius = std::min(cellWidth, cellHeight) * (0.17f + 0.06f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, glowRadius);
						nvgFillColor(args.vg, nvgRGBA(88, 240, 154, int(44.f + 50.f * breath)));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.105f);
						nvgFillColor(args.vg, nvgRGB(98, 235, 154));
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
						if (!module->boardIndexToCoord(destinationIndex, &row, &col)) {
							continue;
						}
						if (module->opponentHintsPreviewActive && module->board[size_t(destinationIndex)] != 0) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.39f + 1.7f;
						float breath = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.1f + phase);
						float glowRadius = std::min(cellWidth, cellHeight) * (0.16f + 0.055f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, glowRadius);
						nvgFillColor(args.vg, nvgRGBA(255, 216, 114, int(36.f + 48.f * breath)));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.092f);
						nvgFillColor(args.vg, nvgRGB(255, 213, 79));
						nvgFill(args.vg);
					}
				}
				if (!module->gameOver && module->lastMove.originIndex >= 0) {
						NVGcolor edgeColor =
							(module->lastMoveSide == module->humanSide()) ? nvgRGB(98, 235, 154) : nvgRGB(255, 213, 79);
					for (int highlightIndex : {module->lastMove.originIndex, module->lastMove.destinationIndex}) {
						int row = 0;
						int col = 0;
						if (!module->boardIndexToCoord(highlightIndex, &row, &col)) {
							continue;
						}
						float phase = float(highlightIndex) * 0.34f + ((module->lastMoveSide == module->humanSide()) ? 0.f : 1.5f);
						float pulse = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 3.8f + phase);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth - 0.5f, row * cellHeight - 0.5f, cellWidth + 1.f, cellHeight + 1.f);
						if (module->lastMoveSide == module->humanSide()) {
							nvgFillColor(args.vg, nvgRGBA(88, 240, 154, int(18.f + 34.f * pulse)));
						}
						else {
							nvgFillColor(args.vg, nvgRGBA(255, 216, 114, int(18.f + 34.f * pulse)));
						}
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth + 1.f, row * cellHeight + 1.f, cellWidth - 2.f, cellHeight - 2.f);
						nvgStrokeColor(args.vg, edgeColor);
						nvgStrokeWidth(args.vg, 2.0f);
						nvgStroke(args.vg);
					}
				}
				if (!module->gameOver && module->captureFlashSeconds > 0.f && module->lastMove.destinationIndex >= 0) {
					int row = 0;
					int col = 0;
					if (module->boardIndexToCoord(module->lastMove.destinationIndex, &row, &col)) {
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
							bool humanPiece = piece > 0;
							if (module->isOthelloMode()) {
								float discR = radius * 1.02f;
								// Human side renders black discs, AI renders white discs.
								NVGcolor edge = humanPiece ? nvgRGBA(28, 28, 32, strokeAlpha) : nvgRGBA(198, 200, 208, strokeAlpha);
								NVGcolor high = humanPiece ? nvgRGBA(96, 100, 114, fillAlpha) : nvgRGBA(254, 254, 255, fillAlpha);
								NVGcolor low = humanPiece ? nvgRGBA(12, 14, 18, fillAlpha) : nvgRGBA(202, 206, 216, fillAlpha);

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
									if (CHESS_ATLAS_ENABLED && drawChessAtlasPiece(args.vg, centerX, centerY, cellWidth, cellHeight, piece, alpha)) {
										return;
									}
									int pieceType = std::abs(piece);
									float pieceR = radius * 2.42f;
									const float chessPieceYOffset = pieceR * 0.03f;
									const float chessOutlineStroke = 1.08f;
									const float kingCrossInnerStroke = chessOutlineStroke;
									const float kingCrossOutlineStroke = 1.92f;
									NVGcolor pieceFill = humanPiece ? nvgRGBA(238, 232, 214, fillAlpha) : nvgRGBA(34, 36, 44, fillAlpha);
									NVGcolor pieceEdge = humanPiece ? nvgRGBA(76, 68, 56, strokeAlpha) : nvgRGBA(214, 220, 234, strokeAlpha);
									NVGcolor pieceDetail = humanPiece ? nvgRGBA(44, 36, 28, fillAlpha) : nvgRGBA(246, 246, 252, fillAlpha);
									NVGcolor pieceContrast = humanPiece ? nvgRGBA(24, 24, 30, strokeAlpha) : nvgRGBA(250, 250, 255, strokeAlpha);
									float baseY = centerY + pieceR * 0.29f;
									nvgSave(args.vg);
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
										NVGcolor crossInner = humanPiece ? nvgRGBA(246, 246, 252, strokeAlpha) : nvgRGBA(18, 18, 24, strokeAlpha);
										nvgStrokeColor(args.vg, crossInner);
										nvgStrokeWidth(args.vg, kingCrossInnerStroke);
										nvgStroke(args.vg);
										break;
									}
									}
									nvgRestore(args.vg);
									return;
								}

						NVGcolor coreInner = humanPiece ? nvgRGBA(237, 112, 94, fillAlpha) : nvgRGBA(78, 78, 86, fillAlpha);
						NVGcolor coreOuter = humanPiece ? nvgRGBA(152, 46, 38, fillAlpha) : nvgRGBA(12, 12, 16, fillAlpha);
						NVGcolor rimBright = humanPiece ? nvgRGBA(255, 228, 208, strokeAlpha) : nvgRGBA(210, 210, 216, strokeAlpha);
						NVGcolor rimDark = humanPiece ? nvgRGBA(82, 22, 16, int(210.f * alpha)) : nvgRGBA(6, 6, 9, int(215.f * alpha));

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
						NVGcolor rimBandColor = humanPiece ? nvgRGBA(112, 38, 30, int(128.f * alpha))
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

							NVGcolor ridgeFillA = humanPiece ? nvgRGBA(248, 176, 154, int(112.f * alpha))
							                                 : nvgRGBA(172, 172, 184, int(94.f * alpha));
							NVGcolor ridgeFillB = humanPiece ? nvgRGBA(198, 104, 86, int(104.f * alpha))
							                                 : nvgRGBA(112, 112, 122, int(86.f * alpha));
							NVGcolor ridgeStroke = humanPiece ? nvgRGBA(86, 24, 18, int(110.f * alpha))
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
						nvgStrokeColor(args.vg, humanPiece ? nvgRGBA(255, 212, 196, int(60.f * alpha))
						                                  : nvgRGBA(196, 196, 206, int(46.f * alpha)));
						nvgStrokeWidth(args.vg, 0.70f);
						nvgStroke(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, rimInnerR);
						nvgStrokeColor(args.vg, humanPiece ? nvgRGBA(70, 18, 14, int(82.f * alpha))
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

						const bool showMovablePieceHints =
							module->highlightMode == Crownstep::HIGHLIGHT_RING
							&& !module->gameOver
							&& module->turnSide == module->humanSide();
					int cellCount = module->boardCellCount();
					std::vector<uint8_t> movableOrigins(size_t(cellCount), 0u);
					if (showMovablePieceHints) {
						for (const Move& move : module->humanMoves) {
							if (move.originIndex >= 0 && move.originIndex < cellCount) {
								movableOrigins[size_t(move.originIndex)] = 1u;
							}
						}
					}

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
						if (!module->boardIndexToCoord(i, &row, &col)) {
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
						if (!module->boardIndexToCoord(startIndex, &row, &col)) {
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
						if (!module->boardIndexToCoord(captureIndex, &row, &col)) {
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
					if (module->boardIndexToCoord(fromIndex, &fromRow, &fromCol) &&
						module->boardIndexToCoord(toIndex, &toRow, &toCol)) {
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
					if (!module->gameOver) {
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
						if (!module->boardIndexToCoord(destinationIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.43f + 0.9f;
						float breath = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.8f + phase);
						float ringRadius = std::min(cellWidth, cellHeight) * (0.21f + 0.05f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, ringRadius);
						nvgStrokeColor(args.vg, nvgRGBA(98, 235, 154, int(186.f + 62.f * breath)));
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
						if (!module->boardIndexToCoord(destinationIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.39f + 2.2f;
						float breath = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.2f + phase);
						float ringRadius = std::min(cellWidth, cellHeight) * (0.205f + 0.05f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, ringRadius);
						nvgStrokeColor(args.vg, nvgRGBA(255, 213, 79, int(180.f + 68.f * breath)));
						nvgStrokeWidth(args.vg, 1.85f);
						nvgStroke(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.062f);
						nvgFillColor(args.vg, nvgRGBA(255, 222, 128, int(190.f + 56.f * breath)));
						nvgFill(args.vg);
					}
				}

					// Human-turn assist: ring pieces that have at least one legal move.
						if (showMovablePieceHints) {
							const bool selectedSquareGlowActive = !module->gameOver && module->selectedSquare >= 0;
							for (int i = 0; i < cellCount; ++i) {
								if (!movableOrigins[size_t(i)]) {
									continue;
								}
							if (selectedSquareGlowActive && i == module->selectedSquare) {
								continue;
							}
							int piece = module->board[size_t(i)];
							if (crownstep::pieceSide(piece) != module->humanSide()) {
								continue;
							}
						int row = 0;
						int col = 0;
						if (!module->boardIndexToCoord(i, &row, &col)) {
							continue;
						}
							float centerX = (col + 0.5f) * cellWidth;
							float centerY = (row + 0.5f) * cellHeight;
							float phase = float(i) * 0.37f + 0.4f;
							float pulse = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.6f + phase);
							float ringRadius = std::min(cellWidth, cellHeight) * (0.43f + 0.03f * pulse);

							nvgBeginPath(args.vg);
							nvgCircle(args.vg, centerX, centerY, ringRadius);
							nvgStrokeColor(args.vg, nvgRGBA(88, 240, 154, int(38.f + 44.f * pulse)));
							nvgStrokeWidth(args.vg, 3.6f);
							nvgStroke(args.vg);

							nvgBeginPath(args.vg);
							nvgCircle(args.vg, centerX, centerY, ringRadius);
							nvgStrokeColor(args.vg, nvgRGBA(98, 235, 154, int(182.f + 58.f * pulse)));
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
		}
		nvgRestore(args.vg);
	}
};

struct CrownstepStepCounterWidget final : Widget {
	Crownstep* module = nullptr;

	explicit CrownstepStepCounterWidget(Crownstep* crownstepModule) {
		module = crownstepModule;
	}

	void draw(const DrawArgs& args) override {
		if (!module) {
			return;
		}

		int totalSteps = module->activeLength();
		int currentStep = module->displayedStep;
		currentStep = clamp(currentStep, 0, totalSteps);

		char stepCounterText[64];
		std::snprintf(stepCounterText, sizeof(stepCounterText), "%d / %d", currentStep, totalSteps);

		const float x = mm2px(Vec(6.6f, 0.f)).x;
		const float y = mm2px(Vec(0.f, 4.0f)).y;
		const float w = mm2px(Vec(26.0f, 0.f)).x;
		const float h = mm2px(Vec(0.f, 4.5f)).y;

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x, y, w, h, 3.5f);
		nvgFillColor(args.vg, nvgRGBA(10, 12, 14, 186));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + 0.5f, y + 0.5f, w - 1.f, h - 1.f, 3.0f);
		nvgStrokeColor(args.vg, nvgRGBA(236, 222, 198, 94));
		nvgStrokeWidth(args.vg, 1.0f);
		nvgStroke(args.vg);

		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFontSize(args.vg, 11.5f);
		nvgFillColor(args.vg, nvgRGB(242, 228, 204));
		nvgText(args.vg, x + w * 0.5f, y + h * 0.53f, stepCounterText, nullptr);
	}
};

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
				boardWidget->box.pos = mm2px(Vec(5.5f, 11.f));
				boardWidget->box.size = mm2px(Vec(80.5f, 80.5f));
			}
			addChild(boardWidget);
			CrownstepStepCounterWidget* stepCounterWidget = new CrownstepStepCounterWidget(module);
			stepCounterWidget->box = box.zeroPos();
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

		Vec seqPos(43.f, 101.5f);
		Vec newGamePos(43.f, 114.0f);
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
			seqPos = Vec(controlX, controlY + controlH * 0.28f);
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
		applyPointOverride("SEQ_LENGTH_PARAM", &seqPos);
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(seqPos), module, Crownstep::SEQ_LENGTH_PARAM));
		addParam(createParamCentered<LEDButton>(mm2px(newGamePos), module, Crownstep::NEW_GAME_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(clockPos), module, Crownstep::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(resetPos), module, Crownstep::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(transposePos), module, Crownstep::TRANSPOSE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(rootCvPos), module, Crownstep::ROOT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(pitchPos), module, Crownstep::PITCH_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(accentPos), module, Crownstep::ACCENT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(modPos), module, Crownstep::MOD_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(eocPos), module, Crownstep::EOC_OUTPUT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(humanLightPos), module, Crownstep::HUMAN_TURN_LIGHT));
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
		menu->addChild(createSubmenuItem("Pitch Data Source", "", [=](Menu* interpretationMenu) {
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
		menu->addChild(createSubmenuItem("Board Value Layout", "", [=](Menu* valueLayoutMenu) {
			for (int i = 0; i < int(BOARD_VALUE_LAYOUT_NAMES.size()); ++i) {
				valueLayoutMenu->addChild(createCheckMenuItem(
					BOARD_VALUE_LAYOUT_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->boardValueLayoutMode == i;
					},
					[=]() {
						if (module) {
							module->boardValueLayoutMode = i;
						}
					}
				));
			}
		}));
		menu->addChild(createSubmenuItem("Divider", "", [=](Menu* dividerMenu) {
			for (int i = 0; i < int(crownstep::PITCH_DIVIDER_NAMES.size()); ++i) {
				dividerMenu->addChild(createCheckMenuItem(
					crownstep::PITCH_DIVIDER_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->pitchDividerMode == i;
					},
					[=]() {
						if (module) {
							module->pitchDividerMode = i;
						}
					}
				));
			}
		}));
	}
};

Model* modelCrownstep = createModel<Crownstep, CrownstepWidget>("Crownstep");
