#include "MusicManager.h"

#ifndef NO_SDL

std::string MusicManager::musAliasesList[75]=
{
    "wmusic1",//0
    "wmusic2",
    "wmusic3",
    "wmusic4",
    "wmusic5",
    "wmusic6",
    "wmusic7",
    "wmusic8",
    "wmusic9",
    "wmusic10",
    "wmusic11",
    "wmusic12",
    "wmusic13",
    "wmusic14",
    "wmusic15",
    "wmusic16",//15
    "smusic",  //16
    "stmusic",
    "tmusic",  //18
    "music1",  //19
    "music2",
    "music3",
    "music4",
    "music5",
    "music6",
    "music7",
    "music8",
    "music9",
    "music10",
    "music11",
    "music12",
    "music13",
    "music14",
    "music15",
    "music16",
    "music17",
    "music18",
    "music19",
    "music20",
    "music21",
    "music22",
    "music23",
    "<reserved>",
    "music25",
    "music26",
    "music27",
    "music28",
    "music29",
    "music30",
    "music31",
    "music32",
    "music33",
    "music34",
    "music35",
    "music36",
    "music37",
    "music38",
    "music39",
    "music40",
    "music41",
    "music42",
    "music43",
    "music44",
    "music45",
    "music46",
    "music47",
    "music48",
    "music49",
    "music50",
    "music51",
    "music52",
    "music53",
    "music54",
    "music55",
    "music56"  //73
};

std::string MusicManager::chunksAliasesList[91]=
{
    "sound1",
    "sound2",
    "sound3",
    "sound4",
    "sound5",
    "sound6",
    "sound7",
    "sound8",
    "sound9",
    "sound10",
    "sound11",
    "sound12",
    "sound13",
    "sound14",
    "sound15",
    "sound16",
    "sound17",
    "sound18",
    "sound19",
    "sound20",
    "sound21",
    "sound22",
    "sound23",
    "sound24",
    "sound25",
    "sound26",
    "sound27",
    "sound28",
    "sound29",
    "sound30",
    "sound31",
    "sound32",
    "sound33",
    "sound34",
    "sound35",
    "sound36",
    "sound37",
    "sound38",
    "sound39",
    "sound40",
    "sound41",
    "sound42",
    "sound43",
    "sound44",
    "sound45",
    "sound46",
    "sound47",
    "sound48",
    "sound49",
    "sound50",
    "sound51",
    "sound52",
    "sound53",
    "sound54",
    "sound55",
    "sound56",
    "sound57",
    "sound58",
    "sound59",
    "sound60",
    "sound61",
    "sound62",
    "sound63",
    "sound64",
    "sound65",
    "sound66",
    "sound67",
    "sound68",
    "sound69",
    "sound70",
    "sound71",
    "sound72",
    "sound73",
    "sound74",
    "sound75",
    "sound76",
    "sound77",
    "sound78",
    "sound79",
    "sound80",
    "sound81",
    "sound82",
    "sound83",
    "sound84",
    "sound85",
    "sound86",
    "sound87",
    "sound88",
    "sound89",
    "sound90",
    "sound91"
};

std::string MusicManager::defaultMusList[75]=
{
    "music\\smb3-world1.mp3",
    "music\\smb3-world4.mp3",
    "music\\smb3-world7.mp3",
    "music\\smw-worldmap.mp3",
    "music\\nsmb-world.mp3",
    "music\\smb3-world2.mp3",
    "music\\smw-forestofillusion.mp3",
    "music\\smb3-world3.mp3",
    "music\\smb3-world8.mp3",
    "music\\smb3-world6.mp3",
    "music\\smb3-world5.mp3",
    "music\\smw-special.mp3",
    "music\\smw-bowserscastle.mp3",
    "music\\smw-starroad.mp3",
    "music\\smw-yoshisisland.mp3",
    "music\\smw-vanilladome.mp3",
    "music\\smw-switch.mp3",
    "music\\smb3-switch.mp3",
    "music\\smg-title.mp3",
    "music\\smb3-overworld.mp3",
    "music\\smb3-sky.mp3",
    "music\\smb3-castle.mp3",
    "music\\smb3-underground.mp3",
    "music\\smb2-overworld.mp3",
    "music\\smb3-boss.mp3",
    "music\\smb-underground.mp3",
    "music\\sf-corneria.mp3",
    "music\\smb-overworld.mp3",
    "music\\smw-overworld.mp3",
    "music\\sm-brinstar.mp3",
    "music\\sm-crateria.mp3",
    "music\\nsmb-overworld.mp3",
    "music\\sm64-desert.mp3",
    "music\\smb2-boss.mp3",
    "music\\mariorpg-forestmaze.mp3",
    "music\\smw-ghosthouse.mp3",
    "music\\smg-beach-bowl-galaxy.mp3",
    "music\\ssbb-airship.mp3",
    "music\\smg-star-reactor.mp3",
    "music\\mariorpg-bowser.mp3",
    "music\\tds-metroid-charge.mp3",
    "<reserved>",
    "music\\z3-lost-woods.mp3",
    "music\\smb2-underground.mp3",
    "music\\mario64-castle.mp3",
    "music\\mario64-maintheme.mp3",
    "music\\smw-sky.mp3",
    "music\\smw-cave.mp3",
    "music\\mariorpg-mariospad.mp3",
    "music\\mariorpg-seasidetown.mp3",
    "music\\mariorpg-tadpolepond.mp3",
    "music\\mariorpg-nimbusland.mp3",
    "music\\mariorpg-rosetown.mp3",
    "music\\mario64-snowmountain.mp3",
    "music\\mario64-boss.mp3",
    "music\\pm-shiver-mountain.mp3",
    "music\\pm-yoshis-village.mp3",
    "music\\ssbb-zelda2.mp3",
    "music\\ssbb-meta.mp3",
    "music\\smw-castle.mp3",
    "music\\smb-castle.mp3",
    "music\\smb2-wart.mp3",
    "music\\sm-itemroom.mp3",
    "music\\sm-brain.mp3",
    "music\\smb-water.mp3",
    "music\\smb3-water.mp3",
    "music\\smw-water.mp3",
    "music\\mario64-water.mp3",
    "music\\mario64-cave.mp3",
    "music\\smw-boss.mp3",
    "music\\ssbb-underground.mp3",
    "music\\ssbb-waluigi.mp3",
    "music\\smb3-hammer.mp3",
    "music\\smg2-fg.mp3",
    "music\\mkwii-mushroom-gorge.mp3"
};

std::string MusicManager::defaultChunksList[91]=
{
    "sound\\player-jump.mp3",
    "sound\\stomped.mp3",
    "sound\\block-hit.mp3",
    "sound\\block-smash.mp3",
    "sound\\player-shrink.mp3",
    "sound\\player-grow.mp3",
    "sound\\mushroom.mp3",
    "sound\\player-died.mp3",
    "sound\\shell-hit.mp3",
    "sound\\player-slide.mp3",
    "sound\\item-dropped.mp3",
    "sound\\has-item.mp3",
    "sound\\camera-change.mp3",
    "sound\\coin.mp3",
    "sound\\1up.mp3",
    "sound\\lava.mp3",
    "sound\\warp.mp3",
    "sound\\fireball.mp3",
    "sound\\level-win.mp3",
    "sound\\boss-beat.mp3",
    "sound\\dungeon-win.mp3",
    "sound\\bullet-bill.mp3",
    "sound\\grab.mp3",
    "sound\\spring.mp3",
    "sound\\hammer.mp3",
    "sound\\slide.mp3",
    "sound\\newpath.mp3",
    "sound\\level-select.mp3",
    "sound\\do.mp3",
    "sound\\pause.mp3",
    "sound\\key.mp3",
    "sound\\pswitch.mp3",
    "sound\\tail.mp3",
    "sound\\racoon.mp3",
    "sound\\boot.mp3",
    "sound\\smash.mp3",
    "sound\\thwomp.mp3",
    "sound\\birdo-spit.mp3",
    "sound\\birdo-hit.mp3",
    "sound\\smb2-exit.mp3",
    "sound\\birdo-beat.mp3",
    "sound\\npc-fireball.mp3",
    "sound\\fireworks.mp3",
    "sound\\bowser-killed.mp3",
    "sound\\game-beat.mp3",
    "sound\\door.mp3",
    "sound\\message.mp3",
    "sound\\yoshi.mp3",
    "sound\\yoshi-hurt.mp3",
    "sound\\yoshi-tongue.mp3",
    "sound\\yoshi-egg.mp3",
    "sound\\got-star.mp3",
    "sound\\zelda-kill.mp3",
    "sound\\player-died2.mp3",
    "sound\\yoshi-swallow.mp3",
    "sound\\ring.mp3",
    "sound\\dry-bones.mp3",
    "sound\\smw-checkpoint.mp3",
    "sound\\dragon-coin.mp3",
    "sound\\smw-exit.mp3",
    "sound\\smw-blaarg.mp3",
    "sound\\wart-bubble.mp3",
    "sound\\wart-die.mp3",
    "sound\\sm-block-hit.mp3",
    "sound\\sm-killed.mp3",
    "sound\\sm-hurt.mp3",
    "sound\\sm-glass.mp3",
    "sound\\sm-boss-hit.mp3",
    "sound\\sm-cry.mp3",
    "sound\\sm-explosion.mp3",
    "sound\\climbing.mp3",
    "sound\\swim.mp3",
    "sound\\grab2.mp3",
    "sound\\smw-saw.mp3",
    "sound\\smb2-throw.mp3",
    "sound\\smb2-hit.mp3",
    "sound\\zelda-stab.mp3",
    "sound\\zelda-hurt.mp3",
    "sound\\zelda-heart.mp3",
    "sound\\zelda-died.mp3",
    "sound\\zelda-rupee.mp3",
    "sound\\zelda-fire.mp3",
    "sound\\zelda-item.mp3",
    "sound\\zelda-key.mp3",
    "sound\\zelda-shield.mp3",
    "sound\\zelda-dash.mp3",
    "sound\\zelda-fairy.mp3",
    "sound\\zelda-grass.mp3",
    "sound\\zelda-hit.mp3",
    "sound\\zelda-sword-beam.mp3",
    "sound\\bubble.mp3"
};

int MusicManager::chunksChannelsList[91] =
{
     0,//"sound\\player-jump.mp3",
    -1,//"sound\\stomped.mp3",
    -1,//"sound\\block-hit.mp3",
    -1,//"sound\\block-smash.mp3",
    -1,//"sound\\player-shrink.mp3",
    -1,//"sound\\player-grow.mp3",
    -1,//"sound\\mushroom.mp3",
    -1,//"sound\\player-died.mp3",
     1,//"sound\\shell-hit.mp3",
     2,//"sound\\player-slide.mp3",
    -1,//"sound\\item-dropped.mp3",
    -1,//"sound\\has-item.mp3",
    -1,//"sound\\camera-change.mp3",
    -1,//"sound\\coin.mp3",
    -1,//"sound\\1up.mp3",
    -1,//"sound\\lava.mp3",
    -1,//"sound\\warp.mp3",
    -1,//"sound\\fireball.mp3",
    -1,//"sound\\level-win.mp3",
    -1,//"sound\\boss-beat.mp3",
    -1,//"sound\\dungeon-win.mp3",
    -1,//"sound\\bullet-bill.mp3",
    -1,//"sound\\grab.mp3",
    -1,//"sound\\spring.mp3",
    -1,//"sound\\hammer.mp3",
    -1,//"sound\\slide.mp3",
     3,//"sound\\newpath.mp3",
    -1,//"sound\\level-select.mp3",
    -1,//"sound\\do.mp3",
    -1,//"sound\\pause.mp3",
    -1,//"sound\\key.mp3",
     4,//"sound\\pswitch.mp3",
    -1,//"sound\\tail.mp3",
    -1,//"sound\\racoon.mp3",
    -1,//"sound\\boot.mp3",
     5,//"sound\\smash.mp3",
    -1,//"sound\\thwomp.mp3",
    -1,//"sound\\birdo-spit.mp3",
    -1,//"sound\\birdo-hit.mp3",
    -1,//"sound\\smb2-exit.mp3",
    -1,//"sound\\birdo-beat.mp3",
    -1,//"sound\\npc-fireball.mp3",
    -1,//"sound\\fireworks.mp3",
    -1,//"sound\\bowser-killed.mp3",
    -1,//"sound\\game-beat.mp3",
    -1,//"sound\\door.mp3",
    -1,//"sound\\message.mp3",
    -1,//"sound\\yoshi.mp3",
    -1,//"sound\\yoshi-hurt.mp3",
     6,//"sound\\yoshi-tongue.mp3",
    -1,//"sound\\yoshi-egg.mp3",
    -1,//"sound\\got-star.mp3",
    -1,//"sound\\zelda-kill.mp3",
    -1,//"sound\\player-died2.mp3",
    -1,//"sound\\yoshi-swallow.mp3",
    -1,//"sound\\ring.mp3",
    -1,//"sound\\dry-bones.mp3",
    -1,//"sound\\smw-checkpoint.mp3",
    -1,//"sound\\dragon-coin.mp3",
    -1,//"sound\\smw-exit.mp3",
    -1,//"sound\\smw-blaarg.mp3",
    -1,//"sound\\wart-bubble.mp3",
    -1,//"sound\\wart-die.mp3",
    -1,//"sound\\sm-block-hit.mp3",
    -1,//"sound\\sm-killed.mp3",
    -1,//"sound\\sm-hurt.mp3",
    -1,//"sound\\sm-glass.mp3",
    -1,//"sound\\sm-boss-hit.mp3",
    -1,//"sound\\sm-cry.mp3",
    -1,//"sound\\sm-explosion.mp3",
     7,//"sound\\climbing.mp3",
     8,//"sound\\swim.mp3",
    -1,//"sound\\grab2.mp3",
    -1,//"sound\\smw-saw.mp3",
    -1,//"sound\\smb2-throw.mp3",
    -1,//"sound\\smb2-hit.mp3",
    -1,//"sound\\zelda-stab.mp3",
    -1,//"sound\\zelda-hurt.mp3",
    -1,//"sound\\zelda-heart.mp3",
    -1,//"sound\\zelda-died.mp3",
    -1,//"sound\\zelda-rupee.mp3",
    -1,//"sound\\zelda-fire.mp3",
    -1,//"sound\\zelda-item.mp3",
    -1,//"sound\\zelda-key.mp3",
    -1,//"sound\\zelda-shield.mp3",
    -1,//"sound\\zelda-dash.mp3",
    -1,//"sound\\zelda-fairy.mp3",
    -1,//"sound\\zelda-grass.mp3",
    -1,//"sound\\zelda-hit.mp3",
    -1,//"sound\\zelda-sword-beam.mp3",
    -1,//"sound\\bubble.mp3"
};




#endif
