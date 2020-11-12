#include "al/LiveActor.hpp"
#include "al/util.hpp"
#include "game/StageScene.hpp"
#include "rs/util.hpp"

void teleportMarioIfNeeded() {
    // We replaced a `MOV X19, X0` instruction with the hook
    // that calls this function; manually reimplement the
    // now-missing instruction here
    __asm ("MOV X19, X0");

    // A reference to the current `StageScene` is in the X0 register;
    // read it into a local variable.
    //
    // Note: Because the `StageScene` reference is in X0, we could also
    //       just accept it as a parameter for this function, since that
    //       parameter would be expected to be passed in X0. Likewise,
    //       we could return `stageScene` at the end of this function to
    //       place it back in X0.
    //
    //       However, that sort of interaction with the hooked code
    //       isn't semantically what's happening here; plus, the values you
    //       need might not always be in the argument registers;
    //       so I'd rather think of this as code that simply reads whatever
    //       state is necessary and restores registers at the end, rather
    //       than pretending it's a normal function call.
    StageScene *stageScene;
    __asm ("MOV %[result], X0" : [result] "=r" (stageScene));

    // Check if Left D-Pad is pressed, using the default
    // controller port (specified by `-1`)
    if (al::isPadTriggerLeft(-1)) {

	// Get the current player actor.
	//
	// Note: The teleportation does not appear to affect captures,
	//       so this presumably always returns a reference to Mario
	//       rather than the currently-controlled actor.
	al::LiveActor *player = rs::getPlayerActor(stageScene);

	// Set the player's vertical translation.
	//
	// Note: Collision is still a factor here, and this will
	//       not teleport Mario through walls.
	//
	//       If you see no change, try triggering the teleportation
	//       during a jump; you may have been seeing no movement on
	//       the ground because the y=0 plane is below you in the current area.
	al::setTransY(player, 0.0);
    }

    // Restore original state by placing
    // `stageScene` back into the X0 register
    __asm ("MOV X0, %[input]" : [input] "=r" (stageScene));
}
