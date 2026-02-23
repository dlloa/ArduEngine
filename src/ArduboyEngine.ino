#include <Arduboy2.h>
#define DEBUG true
#define DEFAULT_FRAMERATE 30

// ENGINE //
#include "gameengine.h"
#include "gamestates.h"
#include "save/savesystem.h"
#include "managers/controller.h"
#include "managers/controllablemanager.h"

////////////
// EEPROM //
////////////
#include "saves.h"

// DEBUG //
#include "debug/debugoverlay.h"
#include "debug/debugmenu.h"

// GAME //
#include "games/arducross.h"
#include "menu/menu.h"
#include "menu/menus.h"

//Counter
#include "games/counter.h"

//////////////////
// Arduboy Data //
//////////////////
Arduboy2 arduboy;
byte _framerate = DEFAULT_FRAMERATE;

//////////////////////
// Game Engine Data //
//////////////////////
FrameCounter _frameCounter = FrameCounter();
Controller controller;
ControllerList cl = ControllerList(&controller);
RenderPipeline renderPipeline;
UpdatePipeline pipeline;
DebugOverlay debugOverlay(&arduboy, &_frameCounter, &pipeline, &renderPipeline, &cl);
DebugMenu    debugMenu(&arduboy, &cl, &pipeline, &renderPipeline, &debugOverlay);
Counter counter = Counter(&arduboy, &cl, &_frameCounter);


////////////////////////////
// Game Specific Managers //
////////////////////////////
StateMachineUpdateable<ControllableManagerStates> sm_main = StateMachineUpdateable<ControllableManagerStates>(&_frameCounter, ControllableManagerStates::GAME_PLAY);
StateMachineControllableWrapper<ControllableManagerStates> sm_main_controller = StateMachineControllableWrapper<ControllableManagerStates>(&cl, &sm_main);
ControllableManager<ControllableManagerStates> gameControllerManager = ControllableManager<ControllableManagerStates>(&cl, &_frameCounter, &sm_main);
RenderableManager<ControllableManagerStates> gameRenderableManager = RenderableManager<ControllableManagerStates>(&_frameCounter, &renderPipeline, &sm_main);
UpdateableManager<ControllableManagerStates> gameUpdateableManager = UpdateableManager<ControllableManagerStates>(&_frameCounter, &pipeline, PIPELINE_GAME, &sm_main);
SceneManager<ControllableManagerStates> sceneManager = SceneManager<ControllableManagerStates>(&_frameCounter, &gameRenderableManager, &gameUpdateableManager, &gameControllerManager, &sm_main);

///////////////
// Game Data //
///////////////
PicrossBoard pb = PicrossBoard(&arduboy, 3);
PicrossBoard apb = PicrossBoard(&arduboy, 3);
PicrossBoardManager* pbm;
Menu* menu;

void setup() {

  ///////////////////
  // Arduboy Setup //
  ///////////////////
  arduboy.begin();
  arduboy.initRandomSeed();
  arduboy.setFrameRate(_framerate);

  /////////////////
  // System Info //
  /////////////////
  debugOverlay.sysInfo.initMemUsage();

  // Save System //
  SaveSystem::init();
  counter.loadData();   // restore saved count on startup

  /////////////////
  // Picross WIP //
  /////////////////
  pb.setBoardValueAtRow(0b010, 0);
  pb.setBoardValueAtRow(0b101, 1);
  pb.setBoardValueAtRow(0b010, 2);

  //Create all Possible Game Objects
  pbm = new PicrossBoardManager(&arduboy, &cl);
  pbm -> loadBoard(&pb);
  SavedPicrossBoard savedBoard;
  SaveSystem::load(SAVED_BOARD, savedBoard);
  pbm -> loadCurrentBoard(savedBoard);

  menu = new Menu(&arduboy, &cl, 3, menuItems);
  menu->setPosition(64,0);

  // Debug names (PROGMEM strings, 2 bytes per object in RAM).
  sceneManager.dbgName                 = PSTR("SceneMgr");
  counter.Updateable::dbgName          = PSTR("Counter");
  counter.Renderable::dbgName          = PSTR("Counter");
  debugOverlay.Renderable::dbgName     = PSTR("Overlay");
  debugMenu.Renderable::dbgName        = PSTR("DebugMenu");

  //Register update pipeline entries
  pipeline.add(PIPELINE_SCENE, &sceneManager);

  // Overlay and debug menu render on top of everything else.
  renderPipeline.add(PIPELINE_OVERLAY, &debugOverlay);
  renderPipeline.add(PIPELINE_OVERLAY, &debugMenu);

  // A+B+LEFT held ~2s persistent binding to open the debug menu.
  cl.addPriorityControl(BUTTON_CHORD_HELD, A_BUTTON | B_BUTTON | LEFT_BUTTON, DebugMenu::OPEN, &debugMenu);
  debugMenu.stateMachine = &sm_main;

  // Counter is active in all states so it always updates (increments over time)
  // regardless of which scene is currently displayed.
  gameUpdateableManager.updateables.add(ControllableManagerStates::GAME_START, &counter);
  gameUpdateableManager.updateables.add(ControllableManagerStates::GAME_PLAY,  &counter);
  gameUpdateableManager.updateables.add(ControllableManagerStates::SETTINGS,   &counter);
  gameUpdateableManager.updateables.add(ControllableManagerStates::GAME_OVER,  &counter);

  // Priority controls are registered once at startup and persist across all
  // state transitions (CONTROL_PERSISTENT priority).
  counter.takePriorityControl();

  //Bind Controllables to State
  gameControllerManager.controllables.bind(ControllableManagerStates::GAME_PLAY, &counter);
  gameControllerManager.controllables.bind(ControllableManagerStates::SETTINGS, menu);

  //Bind Renderables to State
  gameRenderableManager.renderables.add(ControllableManagerStates::GAME_PLAY, &counter);

  gameRenderableManager.renderables.add(ControllableManagerStates::SETTINGS, menu);
  gameRenderableManager.renderables.add(ControllableManagerStates::SETTINGS, &counter);

  gameRenderableManager.renderables.add(ControllableManagerStates::GAME_OVER, &counter);
}

void loop() {
  if (!(arduboy.nextFrame())) return;
  debugOverlay.update();
  arduboy.clear();

  //////////////
  // Controls //
  //////////////

  //Standard arduboy controls update
  arduboy.pollButtons();

  //ArduboyEngine control data processor
  controller.update(arduboy.buttonsState());

 //ArduboyEngine running all currently assigned controls
  cl.runControls();

  ////////////
  // Update //
  ////////////
  pipeline.updateAll();    // PRE → SCENE → GAME

  ////////////
  // Render //
  ////////////
  renderPipeline.renderAll();

  arduboy.display();
}
