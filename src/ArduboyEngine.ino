#include <Arduboy2.h>
#define DEBUG true
#define DEFAULT_FRAMERATE 30

// ENGINE //
#include "gameengine.h"
#include "save/savesystem.h"
#include "controls/controller.h"
#include "controls/controllablemanager.h"

////////////
// EEPROM //
////////////
#include "save/savesystem.h"
#include "saves.h"

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
RenderableSystemInfo _renderablesystem = RenderableSystemInfo(&arduboy, &_frameCounter);
Controller controller;
ControllerList cl = ControllerList(&controller);
RenderList renderlist = RenderList();
Counter counter = Counter(&arduboy, &cl, &_frameCounter);


////////////////////////////
// Game Specific Managers //
////////////////////////////
enum class ControllableManagerStates {
    STATE_MIN,
    GAME_START = STATE_MIN,
    SETTINGS,
    GAME_PLAY,
    GAME_OVER,
    STATE_MAX = GAME_OVER
};

StateMachineUpdateable<ControllableManagerStates> sm_main = StateMachineUpdateable<ControllableManagerStates>(&_frameCounter, ControllableManagerStates::STATE_MIN);
StateMachineControllableWrapper<ControllableManagerStates> sm_main_controller = StateMachineControllableWrapper<ControllableManagerStates>(&cl, &sm_main);
ControllableManager<ControllableManagerStates> gameControllerManager = ControllableManager<ControllableManagerStates>(&cl, &_frameCounter, &sm_main);
RenderableManager<ControllableManagerStates> gameRenderableManager = RenderableManager<ControllableManagerStates>(&_frameCounter, &renderlist, &sm_main);

///////////////
// Game Data //
///////////////
PicrossBoard pb = PicrossBoard(&arduboy, 3);
PicrossBoard apb = PicrossBoard(&arduboy, 3);
PicrossBoardManager* pbm;// = PicrossBoardManager(&arduboy, &cl);
Menu* menu;
MenuStateSetter<ControllableManagerStates>* state_setter_menu;

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
  _renderablesystem.sysInfo.initMemUsage();

  // Save System //
  SaveSystem::init();
  
  //
  // SavedPicrossBoard temp;
  // temp.boardsize = 3;
  // temp.board[0] = 1;
  // temp.board[1] = 2;
  // temp.board[2] = 3;
  // SaveSystem::save(SAVED_BOARD, temp);
  //

  /////////////////
  // Picross WIP //
  /////////////////
  pb.setBoardValueAtRow(0b010, 0);
  pb.setBoardValueAtRow(0b101, 1);
  pb.setBoardValueAtRow(0b010, 2);
  // pb.setBoardValueAtRow(0b010101010101010, 0);
  // pb.setBoardValueAtRow(0b101010101010101, 1);
  // pb.setBoardValueAtRow(0b010101010101010, 2);
  // pb.setBoardValueAtRow(0b101010101010101, 3);
  // pb.setBoardValueAtRow(0b010101010101010, 4);
  // pb.setBoardValueAtRow(0b101010101010101, 5);
  // pb.setBoardValueAtRow(0b010101010101010, 6);
  // pb.setBoardValueAtRow(0b101010101010101, 7);
  // pb.setBoardValueAtRow(0b010101010101010, 8);
  // pb.setBoardValueAtRow(0b101010101010101, 9);
  // pb.setBoardValueAtRow(0b010101010101010, 10);
  // pb.setBoardValueAtRow(0b101010101010101, 11);
  // pb.setBoardValueAtRow(0b010101010101010, 12);
  // pb.setBoardValueAtRow(0b101010101010101, 13);
  // pb.setBoardValueAtRow(0b010101010101010, 14);
  // pb.setBoardValueAtRow(0b101010101010101, 15);

  //Create all Possible Game Objects
  pbm = new PicrossBoardManager(&arduboy, &cl);
  pbm -> loadBoard(&pb);
  SavedPicrossBoard savedBoard;
  SaveSystem::load(SAVED_BOARD, savedBoard);
  pbm -> loadCurrentBoard(savedBoard);

  menu = new Menu(&arduboy, &cl, 3, menuItems);
  menu->setPosition(64,0);

  state_setter_menu = new MenuStateSetter<ControllableManagerStates>(&arduboy, &cl, 4, statemenuItems, &sm_main, &_frameCounter);
  state_setter_menu->setPosition(64, 0);

  //Priority controls
  sm_main_controller.takeControl();
  counter.takePriorityControl();

  //Bind Controllables to State
  gameControllerManager.controllables.bind(ControllableManagerStates::GAME_START, state_setter_menu);
  gameControllerManager.controllables.bind(ControllableManagerStates::GAME_PLAY, &counter);
  gameControllerManager.controllables.bind(ControllableManagerStates::SETTINGS, menu);

  //Bind Renderables to State
  gameRenderableManager.renderables.add(ControllableManagerStates::GAME_START, state_setter_menu);
  gameRenderableManager.renderables.add(ControllableManagerStates::GAME_START, &counter);

  gameRenderableManager.renderables.add(ControllableManagerStates::GAME_PLAY, &counter);

  gameRenderableManager.renderables.add(ControllableManagerStates::SETTINGS, menu);
  gameRenderableManager.renderables.add(ControllableManagerStates::SETTINGS, &counter);

  gameRenderableManager.renderables.add(ControllableManagerStates::GAME_OVER, &_renderablesystem);
  gameRenderableManager.renderables.add(ControllableManagerStates::GAME_OVER, &counter);

  //TODO: temp setting to min?
  // gameControllerManager.stateMachine.setState(ControllableManagerStates::STATE_MIN);
}

void loop() {
  if (!(arduboy.nextFrame())) return;
  _renderablesystem.update();
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
  //TODO: Some kind of update list, Updateables list. Like renderList.
  //Apply rulles of what needs to be updated before and after
  //StateSetter, doesn't need to run every frame.

  //On Menu madeSelection, will update State Machine to selected item in Menu.
  //Marks the start of the state machine transition, isTransitionFinished() = false
  state_setter_menu->update();

  //on !isTransitionFinished() will clear and swap in renderables for this new state.
  gameRenderableManager.update();

  //on !isTransitionFinished() will clear non-priority controls and swap in new ones.
  gameControllerManager.update();

  //
  counter.update();
  //

  //State Machine should be updated last, it will complete the state transition. isTransitionFinished() = true
  sm_main.update();


  ////////////
  // Render //
  ////////////
  renderlist.renderAll();

  ///////////
  // Debug //
  ///////////
  String debugPrint = "";
  //gameControllerManager.stateMachine
  // debugPrint += gameControllerManager.stateMachine.getState() == ControllableManagerStates::GAME_START ? "1" : "0";
  // debugPrint += gameControllerManager.stateMachine.getState() == ControllableManagerStates::SETTINGS ? "1" : "0";
  // debugPrint += gameControllerManager.stateMachine.getState() == ControllableManagerStates::GAME_PLAY ? "1" : "0";
  // debugPrint += gameControllerManager.stateMachine.getState() == ControllableManagerStates::GAME_OVER ? "1" : "0";

  debugPrint += sm_main.getState() == ControllableManagerStates::GAME_START ? "1" : "0";
  debugPrint += sm_main.getState() == ControllableManagerStates::SETTINGS ? "1" : "0";
  debugPrint += sm_main.getState() == ControllableManagerStates::GAME_PLAY ? "1" : "0";
  debugPrint += sm_main.getState() == ControllableManagerStates::GAME_OVER ? "1" : "0";
  debugPrint += "-";
  debugPrint += cl.numControls;
  debugPrint += "-";
  debugPrint += renderlist.nNumRenderable;
  arduboy.setCursor(0, 56);
  arduboy.print(debugPrint);

  arduboy.display();
}
