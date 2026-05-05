#include "plugin.hpp"
#include "PanelSvgUtils.hpp"
#include <vector>
#include <algorithm>

struct Sil : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		INPUT_L_INPUT,
		INPUT_R_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_L_OUTPUT,
		OUTPUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	static constexpr int HISTOGRAM_BINS = 1000;
	static constexpr float HISTOGRAM_DURATION = 10.f;

	struct HistogramData {
		float minL[HISTOGRAM_BINS] = {};
		float maxL[HISTOGRAM_BINS] = {};
		float minR[HISTOGRAM_BINS] = {};
		float maxR[HISTOGRAM_BINS] = {};
		int writePtr = 0;

		float currentMinL = 1e10f, currentMaxL = -1e10f;
		float currentMinR = 1e10f, currentMaxR = -1e10f;
		int samplesInCurrentBin = 0;
		int samplesPerBin = 441;

		float smoothedPeak = 5.f;
	} hist;

	static constexpr int SPEC_FREQ_BINS = 128;
	static constexpr int FFT_SIZE = 2048;

	struct SpectrumData {
		float magnitudesL[SPEC_FREQ_BINS] = {};
		float magnitudesR[SPEC_FREQ_BINS] = {};
		
		float bufferL[FFT_SIZE] = {};
		float bufferR[FFT_SIZE] = {};
		int writePtr = 0;

		alignas(16) float window[FFT_SIZE];
		alignas(16) float fftInL[FFT_SIZE];
		alignas(16) float fftInR[FFT_SIZE];
		alignas(16) float fftOutL[FFT_SIZE];
		alignas(16) float fftOutR[FFT_SIZE];

		dsp::RealFFT* fft = nullptr;

		float smoothedPeakDb = 0.f;
	} spec;

	dsp::ClockDivider specDivider;

	Sil() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(INPUT_L_INPUT, "Left");
		configInput(INPUT_R_INPUT, "Right");
		configOutput(OUTPUT_L_OUTPUT, "Left");
		configOutput(OUTPUT_R_OUTPUT, "Right");

		hist.samplesPerBin = (int)(APP->engine->getSampleRate() * HISTOGRAM_DURATION / HISTOGRAM_BINS);
		
		spec.fft = new dsp::RealFFT(FFT_SIZE);
		for (int i = 0; i < FFT_SIZE; i++) {
			spec.window[i] = 0.5f - 0.5f * std::cos(2.f * M_PI * i / (FFT_SIZE - 1));
		}

		specDivider.setDivision(2048); // Update spectrum roughly 20-30 times per second
	}

	~Sil() {
		delete spec.fft;
	}

	void onSampleRateChange(const SampleRateChangeEvent& e) override {
		hist.samplesPerBin = (int)(e.sampleRate * HISTOGRAM_DURATION / HISTOGRAM_BINS);
		if (hist.samplesPerBin < 1) hist.samplesPerBin = 1;
	}

	void process(const ProcessArgs& args) override {
		const float inL = inputs[INPUT_L_INPUT].getVoltage();
		const float inR = inputs[INPUT_R_INPUT].getVoltage();
		outputs[OUTPUT_L_OUTPUT].setChannels(1);
		outputs[OUTPUT_R_OUTPUT].setChannels(1);
		outputs[OUTPUT_L_OUTPUT].setVoltage(inL);
		outputs[OUTPUT_R_OUTPUT].setVoltage(inR);

		// Update histogram (Waveform)
		hist.currentMinL = std::min(hist.currentMinL, inL);
		hist.currentMaxL = std::max(hist.currentMaxL, inL);
		hist.currentMinR = std::min(hist.currentMinR, inR);
		hist.currentMaxR = std::max(hist.currentMaxR, inR);
		hist.samplesInCurrentBin++;

		if (hist.samplesInCurrentBin >= hist.samplesPerBin) {
			hist.minL[hist.writePtr] = hist.currentMinL;
			hist.maxL[hist.writePtr] = hist.currentMaxL;
			hist.minR[hist.writePtr] = hist.currentMinR;
			hist.maxR[hist.writePtr] = hist.currentMaxR;
			hist.writePtr = (hist.writePtr + 1) % HISTOGRAM_BINS;

			// Update smoothed peak for histogram (dynamic zoom)
			float instantPeak = std::max({std::abs(hist.currentMinL), std::abs(hist.currentMaxL), std::abs(hist.currentMinR), std::abs(hist.currentMaxR)});
			// Slow decay, fast attack
			if (instantPeak > hist.smoothedPeak) 
				hist.smoothedPeak = hist.smoothedPeak * 0.9f + instantPeak * 0.1f;
			else
				hist.smoothedPeak = hist.smoothedPeak * 0.999f + instantPeak * 0.001f;
			
			// Keep within reasonable bounds
			hist.smoothedPeak = clamp(hist.smoothedPeak, 0.5f, 12.f);

			hist.currentMinL = 1e10f; hist.currentMaxL = -1e10f;
			hist.currentMinR = 1e10f; hist.currentMaxR = -1e10f;
			hist.samplesInCurrentBin = 0;
		}

		// Update Spectrum Ring Buffer
		spec.bufferL[spec.writePtr] = inL;
		spec.bufferR[spec.writePtr] = inR;
		spec.writePtr = (spec.writePtr + 1) % FFT_SIZE;

		if (specDivider.process()) {
			// Perform FFT (Throttled)
			for (int i = 0; i < FFT_SIZE; i++) {
				int idx = (spec.writePtr + i) % FFT_SIZE;
				spec.fftInL[i] = spec.bufferL[idx] * spec.window[i];
				spec.fftInR[i] = spec.bufferR[idx] * spec.window[i];
			}

			spec.fft->rfft(spec.fftInL, spec.fftOutL);
			spec.fft->rfft(spec.fftInR, spec.fftOutR);

			auto getMagnitude = [&](float* fftOut, int bin) {
				if (bin <= 0) return std::abs(fftOut[0]);
				if (bin >= FFT_SIZE / 2) return std::abs(fftOut[1]);
				float re = fftOut[2 * bin];
				float im = fftOut[2 * bin + 1];
				return std::sqrt(re * re + im * im);
			};

			// Logarithmic mapping: 20Hz to 20kHz
			float sampleRate = args.sampleRate;
			float maxMag = 0.f;
			for (int i = 0; i < SPEC_FREQ_BINS; i++) {
				float f01 = (float)i / (SPEC_FREQ_BINS - 1);
				float hz = 20.f * std::pow(1000.f, f01);
				float bin = hz / (sampleRate / FFT_SIZE);
				
				int binIdx = (int)bin;
				float magL, magR;
				if (binIdx < FFT_SIZE / 2 - 1) {
					float frac = bin - binIdx;
					magL = (1.f - frac) * getMagnitude(spec.fftOutL, binIdx) + frac * getMagnitude(spec.fftOutL, binIdx + 1);
					magR = (1.f - frac) * getMagnitude(spec.fftOutR, binIdx) + frac * getMagnitude(spec.fftOutR, binIdx + 1);
				} else {
					magL = getMagnitude(spec.fftOutL, FFT_SIZE / 2);
					magR = getMagnitude(spec.fftOutR, FFT_SIZE / 2);
				}

				// Normalize magnitudes: 0dB = 10V sine peak (10 * 1024)
				magL /= 10240.f;
				magR /= 10240.f;

				// Faster smoothing for individual bins
				spec.magnitudesL[i] = spec.magnitudesL[i] * 0.3f + magL * 0.7f;
				spec.magnitudesR[i] = spec.magnitudesR[i] * 0.3f + magR * 0.7f;
				maxMag = std::max({maxMag, spec.magnitudesL[i], spec.magnitudesR[i]});
			}

			float instantPeakDb = 20.f * std::log10(maxMag + 1e-6f);
			if (instantPeakDb > spec.smoothedPeakDb)
				spec.smoothedPeakDb = spec.smoothedPeakDb * 0.1f + instantPeakDb * 0.9f; // Fast attack
			else
				spec.smoothedPeakDb = spec.smoothedPeakDb * 0.995f + instantPeakDb * 0.005f; // Slow decay
			
			spec.smoothedPeakDb = clamp(spec.smoothedPeakDb, -100.f, 20.f);
		}
	}
};

struct HistogramWidget : TransparentWidget {
	Sil* module;

	void draw(const DrawArgs& args) override {
		if (!module) return;

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 255));
		nvgFill(args.vg);

		float midY = box.size.y / 2.f;
		float halfH = box.size.y / 4.f;

		NVGcolor cyanColor = nvgRGBA(0x1c, 0xcc, 0xd9, 0xff);
		NVGcolor purpleColor = nvgRGBA(0x7a, 0x5c, 0xff, 0xff);

		auto drawChannel = [&](const float* minBuf, const float* maxBuf, float centerY) {
			for (int i = 0; i < Sil::HISTOGRAM_BINS; i++) {
				int idx = (module->hist.writePtr + i) % Sil::HISTOGRAM_BINS;
				float x = (float)i / (Sil::HISTOGRAM_BINS - 1) * box.size.x;
				// Fixed +/- 10V scale
				float valMin = clamp(minBuf[idx] / 10.f, -1.f, 1.f);
				float valMax = clamp(maxBuf[idx] / 10.f, -1.f, 1.f);
				float yMin = centerY - valMin * halfH;
				float yMax = centerY - valMax * halfH;
				float amp = std::max(std::abs(valMin), std::abs(valMax));
				NVGcolor color = nvgLerpRGBA(purpleColor, cyanColor, amp);

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x, yMin);
				nvgLineTo(args.vg, x, yMax);
				nvgStrokeColor(args.vg, color);
				nvgStrokeWidth(args.vg, 1.0f);
				nvgStroke(args.vg);
			}
		};

		drawChannel(module->hist.minL, module->hist.maxL, midY * 0.5f);
		drawChannel(module->hist.minR, module->hist.maxR, midY * 1.5f);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0, midY);
		nvgLineTo(args.vg, box.size.x, midY);
		nvgStrokeColor(args.vg, nvgRGBA(0x1c, 0xca, 0xd8, 0x40));
		nvgStrokeWidth(args.vg, 0.5f);
		nvgStroke(args.vg);
	}
};

struct SpectrumWidget : TransparentWidget {
	Sil* module;
	bool isRightChannel = false;

	void draw(const DrawArgs& args) override {
		if (!module) return;

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 255));
		nvgFill(args.vg);

		NVGcolor cyanColor = nvgRGBA(0x1c, 0xcc, 0xd9, 0xff);
		NVGcolor purpleColor = nvgRGBA(0x7a, 0x5c, 0xff, 0xff);

		const float* magnitudes = isRightChannel ? module->spec.magnitudesR : module->spec.magnitudesL;
		float barW = box.size.x / Sil::SPEC_FREQ_BINS;

		float ceilingDb = module->spec.smoothedPeakDb + 6.f; // 6dB headroom
		float floorDb = ceilingDb - 70.f; // 70dB range

		for (int i = 0; i < Sil::SPEC_FREQ_BINS; i++) {
			float mag = magnitudes[i];
			// Convert to dB-like scale for better visualization: log10(mag)
			float db = 20.f * std::log10(mag + 1e-6f);
			// Map floor..ceiling to 0..1
			float norm = clamp((db - floorDb) / (ceilingDb - floorDb), 0.f, 1.f);
			
			if (norm <= 0.01f) continue;

			float barH = norm * box.size.y;
			float x = (float)i * barW;
			NVGcolor color = nvgLerpRGBA(purpleColor, cyanColor, norm);

			nvgBeginPath(args.vg);
			nvgRect(args.vg, x, box.size.y - barH, barW - 0.5f, barH);
			nvgFillColor(args.vg, color);
			nvgFill(args.vg);
		}
	}
};

struct BananutBlack : app::SvgPort {
	BananutBlack() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg")));
	}
};

struct SilWidget : ModuleWidget {
	SilWidget(Sil* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/sil.svg");
		setPanel(createPanel(asset::plugin(pluginInstance, "res/sil.svg")));

		math::Rect histRect;
		if (panel_svg::loadRectFromSvgMm(panelPath, "HISTOGRAM", &histRect)) {
			histRect = histRect.grow(Vec(-0.2f, -0.2f));
			HistogramWidget* hw = createWidget<HistogramWidget>(mm2px(histRect.pos));
			hw->box.size = mm2px(histRect.size);
			hw->module = module;
			addChild(hw);
		}

		math::Rect specLRect;
		if (panel_svg::loadRectFromSvgMm(panelPath, "SPECTROGRAM_LEFT", &specLRect)) {
			specLRect = specLRect.grow(Vec(-0.2f, -0.2f));
			SpectrumWidget* sw = createWidget<SpectrumWidget>(mm2px(specLRect.pos));
			sw->box.size = mm2px(specLRect.size);
			sw->module = module;
			sw->isRightChannel = false;
			addChild(sw);
		}

		math::Rect specRRect;
		if (panel_svg::loadRectFromSvgMm(panelPath, "SPECTROGRAM_RIGHT", &specRRect)) {
			specRRect = specRRect.grow(Vec(-0.2f, -0.2f));
			SpectrumWidget* sw = createWidget<SpectrumWidget>(mm2px(specRRect.pos));
			sw->box.size = mm2px(specRRect.size);
			sw->module = module;
			sw->isRightChannel = true;
			addChild(sw);
		}

		Vec inputLPos(26.f, 118.f);
		Vec inputRPos(42.f, 118.f);
		Vec outputLPos(58.f, 118.f);
		Vec outputRPos(74.f, 118.f);

		auto applyPointOverride = [&](const char* elementId, Vec* outPos) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
				*outPos = pointMm;
			}
		};

		applyPointOverride("INPUT_L_INPUT", &inputLPos);
		applyPointOverride("INPUT_R_INPUT", &inputRPos);
		applyPointOverride("OUTPUT_L_OUTPUT", &outputLPos);
		applyPointOverride("OUTPUT_R_OUTPUT", &outputRPos);

		addInput(createInputCentered<PJ301MPort>(mm2px(inputLPos), module, Sil::INPUT_L_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(inputRPos), module, Sil::INPUT_R_INPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(outputLPos), module, Sil::OUTPUT_L_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(outputRPos), module, Sil::OUTPUT_R_OUTPUT));
	}
};

Model* modelSil = createModel<Sil, SilWidget>("Sil");
