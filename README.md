# Automatic Pokemon Breeding for Sword/Shield

I've added multiple loop sequences, dpad buttons, and changed the commands to automatically breed 1 box worth of pokemon.

In order to use this script, you need to use the breeder on route 5.  Set your box so that there's an empty box to the left of the current one.  The current box can also be empty or you can stock it with any amount of eggs.  Stand somewhere in the middle of the road near the bridge, disconnect your controller, and then connect your arduino or whatever compatable device you're using.  You may need to change the number of loops needed for the first and second command sequences depending on what you're breeding and how long it takes to hatch.  Refer to the past projects for more information.

---

#### Thanks

Thanks to https://github.com/kidGodzilla/woff-grinder for updating the project and enabling the use of Arduinos.

Thanks to https://github.com/bertrandom/snowball-thrower for the updated information which modifies the original script to throw snowballs in Zelda. This C Source is much easier to start from, and has a nice object interface for creating new command sequences.

Thanks to Shiny Quagsire for his [Splatoon post printer](https://github.com/shinyquagsire23/Switch-Fightstick) and progmem for his [original discovery](https://github.com/progmem/Switch-Fightstick).

Thanks to [exsilium](https://github.com/bertrandom/snowball-thrower/pull/1) for improving the command structure, optimizing the waiting times, and handling the failure scenarios. It can now run indefinitely!
