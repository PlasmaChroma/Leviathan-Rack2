 ### ⚠️ Issues worth discussing                                                                                                                                                                           
                                                                                                                                                                                                          
  1.  glTexImage2D  called every frame — should be  glTexSubImage2D                                                                                                                                       
                                                                                                                                                                                                          
  At BifurxGL.cpp:                                                                                                                                                                                      
                                                                                                                                                                                                          
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kCurvePointCount, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels.data());
  
   glTexImage2D  re-allocates the texture storage on the driver every call. The correct call for updating an existing texture is  glTexSubImage2D . This costs exactly what  glTexImage2D  costs to avoid 
  — a potential implicit driver-side reallocation every frame. Since the texture is always  kCurvePointCount × 1 , it should be allocated once in  ensureTextureShaderReady()  with  glTexImage2D(...     
  nullptr) , then updated every frame with  glTexSubImage2D .
  
  2.  std::vector<PixelRGBA>  heap-allocated every frame
  
  At BifurxGL.cpp:
  
    std::vector<PixelRGBA> texels(kCurvePointCount);
  
  This is a 513-element heap allocation inside  drawFramebuffer()  on every frame. It should be a persistent member of the widget struct, similar to how  fillVertices  etc. are reserved up-front, so no 
  allocations happen at draw time.
  
  3. The texture crest blending in the shader is not quite equivalent to the original
  
  The original CPU path drew the Gaussian crest stroke on top of the already-blended fill as a separate additive-ish draw call. The new shader computes both and then manually premultiplies them together
  at BifurxGL.cpp:
  
    vec3 blendedColor = mix(cFill.rgb, cCrest.rgb, cCrest.a);
    float blendedAlpha = cFill.a + cCrest.a * (1.0 - cFill.a);
  
  This is standard "over" compositing, which is correct conceptually, but the original drew the crest as a second blended draw pass after the fill was already in the framebuffer. The behaviour is       
  similar but not identical — particularly where  fillAlpha  is near zero (above the curve in the feather region) and the crest Gaussian peaks. Worth a visual check to confirm the crest brightness looks
  right.
  
  4. The quad covers  0  to  spectrumBottomY , not to  spectrumTopY 
  
  The quad goes from  y = 0  to  y = spectrumBottomY . The fragment shader then  discard s anything below  spectrumBottomY . Fragments above the curve but above  spectrumTopY  will evaluate the crest   
  Gaussian — there's no upper scissor in the shader. In the original path the fill only existed within segments where  energy > 0.005 , but there was no hard upper clip. This is likely fine in practice 
  but worth noting — the  spectrumTopY  padding at the top of the plot isn't guarded in the fragment shader the way it was implicitly guarded by the vertex generation loop.
  
  5. The  fillVertices  etc. persistent vectors are still cleared every frame but now never used in shader mode
  
  Lines BifurxGL.cpp still unconditionally  clear()  all the CPU geometry vectors even when  useShaderRenderer  is true. This is a minor extra cost (clearing empty reserved vectors is essentially     
  free), but the capacity reservation in the constructor for those vectors is also wasted memory in shader mode. Not a bug, just slightly inelegant.
  ──────
  ### Summary of recommendations
  
   Issue                                                            │ Severity                                                         │ Fix
  ──────────────────────────────────────────────────────────────────┼──────────────────────────────────────────────────────────────────┼──────────────────────────────────────────────────────────────────
    glTexImage2D  every frame instead of  glTexSubImage2D           │ Medium — causes unnecessary driver allocation traffic            │ Pre-allocate in init, update with  glTexSubImage2D 
    std::vector<PixelRGBA>  heap-allocated per frame                │ Low-Medium — defeats zero-alloc goal of the draw loop            │ Make it a persistent member
   Crest blending not identical to original two-pass behaviour      │ Low — visual correctness question                                │ Visual A/B check recommended
   No upper clip at  spectrumTopY  in shader                        │ Low — mostly cosmetic                                            │ Could add a discard or uniform for it
  

