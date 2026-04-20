We currently have an issue in TD.Scope where dragging on the waveform with the mouse Up/Down does not end up doing a 1:1 time movement relative to the scope itself when we are at unity sensitivity (1.00x)

A temporary hack was put in place with this scalar multiplier of 0.5f to try to bring it more in alignment, and it's close but not perfect.

float sensitivity = (hasLastGoodMsg ? lastGoodMsg.scratchSensitivity : 1.f) * 0.5f;

What we need to model is exactly 1:1 waveform to time drag (only considering the Y axis on scope module).  We need to structure an actual fix that works both at sensitivity 1.0 (in Temporal Deck's setting), but then also be able to scale it appropriately down to 0.5x to 2.00x in the setting.  These scalar values should be applied against the ground truth working offsets at unity sens.

Also, we still want to preserve the audio behavior itself when dragging as it does currently, which is basically the same effect we have going on the record platter dragging.  The intent is not to change the way it sounds, but to make the target landing point line up with where the user was in the waveform.