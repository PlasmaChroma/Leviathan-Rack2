✦ The Wyrm module is a sophisticated wavetable oscillator with a unique "rock" resolution system and a "sand" visualization backend. After a deep dive into the implementation, here is a review focusing on
  correctness, robustness, and performance.

  1. Correctness & Logic Issues

   * Destructive Resampling in setPointCount: In Wyrm.cpp, changing the point count (e.g., via the context menu) calls setFactoryShape(), which completely overwrites any custom user-drawn waveform with a factory
     preset.
       * Recommendation: Implement a resampling step to preserve the user's custom waveform when changing resolution.
   * Wavetable Auto-Normalization: rebuildWavetable() normalizes the table to 1.0 peak amplitude. This makes it impossible to create "quiet" wavetables or use the waveform for unipolar/small-range modulation
     without external attenuation.
       * Recommendation: Make normalization optional or allow the user to scale the overall wave amplitude.
   * Sync Logic and Phase Direction: In process(), hard sync resets phaseDir[c] = 1.0f. However, if the oscillator was running backwards (via soft sync), a hard sync will suddenly flip the direction. This might
     be intended, but usually, hard sync only resets the phase position.

  2. Robustness & Thread Safety

   * Atomic Access Inconsistency: While wavePoints is atomic, the wavetableMip and rocks arrays are not.
       * The audio thread writes to wavetableMip in rebuildWavetable() while potentially reading from it in lookupWave(). Since rebuildWavetable() is called within process(), this is safe unless a UI component
         (like a secondary display) also calls lookupWave(). Currently, UI components use catmullPeriodic, so they are safe.
       * The rocks array is modified directly by the UI thread (in moveRockFromMouse). The audio thread uses a double-buffered activeRockState, which is robust. however, the rebuildRockBoundaryCache call on the
         UI thread modifies the cache used by the audio thread snapshots if not careful.
   * Fast Phase Wrapping: wrap01Fast is used in the audio loop. It only handles a single wrap ($[-1, 1]$ range). While phase increments are typically small, extreme FM or sync settings could potentially push the
     phase further, leading to discontinuities.
   * NaN/Inf Protection: The use of finiteOr and clamp is extensive and provides excellent protection against signal "explosions."

  3. Performance Analysis

  Audio Thread (Critical)
   * Redundant resolveAgainstRocks Work: The resolveAgainstRocks function contains a 3-pass loop. Inside this loop, it redundantly calculates dx, rx, and checks the boundary cache for the same rock multiple
     times per sample.
       * Optimization: Pre-calculate the lower and upper bounds for all active rocks at the current phase once per sample. Then run the 3-pass resolution logic using those fixed bounds.
   * MIP Map Selection: The while loop in lookupWave to find the LOD level can be replaced with a single std::log2 or a bit-manipulation trick for the constant-size 8-level MIP map.

  UI Thread (Heavy)
   * Body Path Rendering Overhead: WyrmWaveEditor::draw and WyrmSandGlWidget (OpenGL) perform extremely heavy calculations.
       * The OpenGL backend calculates up to 8192 samples, each calling resolveAgainstRocks twice.
       * 8192 × 2 × 3  passes × 6  rocks ≈ 300,000 rock checks per frame.
       * Optimization: Cache the "slithered" path points more aggressively or use a lower resolution for the visual slither animation.
   * Sand Simulation Costs: WyrmSand::ensureImageRaster renders a pixel-by-pixel image on the UI thread, calling hashUnit (a multi-step bitwise hash) for every pixel. For a large editor window, this is a
     significant CPU hit.
       * Optimization: Use a pre-computed noise texture for the "sparkles" or only update dirty regions of the raster.
   * Disturbance Logic: disturbSegment is called for every segment of the waveform if slither is active. This performs a bounding-box scan of the sand field on the UI thread.

  4. Summary of Suggested Improvements

   1. Surgical Optimization: Optimize the 3-pass resolution loop in the audio thread by hoisting boundary calculations out of the pass loop.
   2. UI Caching: Improve the caching of the rendered body path. The slither animation is predictable; you could potentially render a few "frames" of the slither offset and interpolate.
   3. Sand Backend: Move the pixel-level sand rasterization to a fragment shader (which the WYRM_RENDER_OPENGL_SHDR mode partially attempts) to offload the UI thread.
   4. Resampling: Update setPointCount to resample the current wavePoints instead of resetting them.

