#ifndef ARDUCROSS
#define ARDUCROSS

#include "../gameengine.h"
#include "../controls/controller.h"
#include "../utilities/utilities.h"

#define MAXBOARDSIZE 15

enum class GameState {
	STATE_MIN,
	MENU_TITLE = STATE_MIN,
	SETTINGS,
	GAME_PLAY,
	PAUSED,
	STATE_MAX = PAUSED
};

enum class BoardState {
	STATE_MIN,
	PLAYING,
	COMPLETE,
	FAIL,
	STATE_MAX = FAIL
};

class PicrossBoard : public Renderable {
public:
	PicrossBoard(Arduboy2* inArduboy, byte inBoardSize) : Renderable(inArduboy), boardSize(inBoardSize){
		// boardSize = inBoardSize;
		board = new uint16_t[boardSize];
		for(int i = 0; i < boardSize; i++){
			board[i] = 0;
		}
	}

	void setBoard(PicrossBoard inBoard){
		boardSize = inBoard.boardSize;
		for(int i = 0; i < boardSize; i++ ){
			board[i] = inBoard.board[i];
		}
	}

	void setBoard(SavedPicrossBoard inBoard){
		boardSize = inBoard.boardsize;
		for(int i = 0; i < boardSize; i++ ){
			board[i] = inBoard.board[i];
		}
	}

	void setBoardValueAtRow(uint16_t inValue, uint8_t inRow){
		if( inRow < boardSize ){
			board[inRow] = inValue;
		}
	}

	bool validPosition(uint8_t xPos, uint8_t yPos){
		return xPos < boardSize && yPos < boardSize;
	}
	void clearBitAt(uint8_t xPos, uint8_t yPos){
		if(!validPosition(xPos, yPos)) return;
        // board[yPos] &= ~( 1 << xPos );
        board[yPos] &= ~(1 << ((boardSize - 1) - xPos));
	}
	void setBitAt(uint8_t xPos, uint8_t yPos){
		if(!validPosition(xPos, yPos)) return;
        // board[yPos] |= ( 1 << xPos );
        board[yPos] |= (1 << ((boardSize - 1) - xPos));
	}
	void toggleBitAt(uint8_t xPos, uint8_t yPos){
		if(!validPosition(xPos, yPos)) return;
		// board[yPos] ^= (1 << xPos); //Little Endian
		board[yPos] ^= (1 << ((boardSize - 1) - xPos)); //Big Endian
	}

	//TODO: int return to return null type value?
	bool getBitAt(uint8_t xPos, uint8_t yPos){
		if( !validPosition(xPos, yPos)) return;
		uint8_t revXPos = (boardSize - 1) - xPos;
		uint16_t xMask = 0b0000000000000001 << revXPos;
		return xMask & board[yPos];
	}

	void setPos(uint8_t x, uint8_t y){
		xPos = x;
		yPos = y;
	}
	bool BoardCompatible(PicrossBoard* inBoard){
		return boardSize == inBoard -> boardSize;
	}

	bool BoardEqual(PicrossBoard* inBoard){
		uint16_t mask = 0b0111111111111111 >> (15 - boardSize);
		for( uint8_t boardIndex = 0; boardIndex < boardSize; boardIndex++ ){
			if( (board[boardIndex] & mask) != (inBoard -> board[boardIndex] & mask)){
				return false;
			}
		}
		return true;
	}

	uint8_t getAutoBlockSize(){
		return 62/boardSize;
	}
	uint8_t getBorderSize(){
		return borderSize;
	}
	uint8_t getXPos(){
		return xPos;
	}
	uint8_t getYPos(){
		return yPos;
	}

	void render() override{
		// arduboy -> setCursor(128 - (blockSize * boardSize), 64 - (blockSize * boardSize));
		// arduboy -> fillRect()

		//Auto Sizing?
		//Max Size = 64
		//BorderSize = 1 * 2 for two sides
		//Inner Space = 62 MAX
		//BlockSize = 62 / BoardSize
		if( autoSize ){
			blockSize = 62 / boardSize;
		}

		arduboy -> drawRect(xPos, yPos, (blockSize * boardSize) + (borderSize * 2), (blockSize * boardSize) + 2);
		for( int yVert = 0; yVert < boardSize; yVert++ ){
			for( int xHorz = boardSize - 1; xHorz >= 0; xHorz-- ){
			// for( int xHorz = 0; xHorz < boardSize; xHorz++ ){
				uint16_t xMask = 0b0000000000000001 << xHorz;
				bool filled = xMask & board[yVert];

				//
				uint8_t revXHorz = (boardSize - 1) - xHorz;

				if( filled ){
					//Empty Box with Cross
					// arduboy -> drawRect( 
					// 	(borderSize + xPos) + revXHorz * blockSize, 
					// 	(borderSize + yPos) + yVert * blockSize, 
					// 	blockSize, blockSize, 
					// 	WHITE);
					arduboy -> drawLine(
						(borderSize + xPos) + revXHorz * blockSize, 
						(borderSize + yPos) + yVert * blockSize, 

						((borderSize + xPos) + revXHorz * blockSize) + blockSize, 
						((borderSize + yPos) + yVert * blockSize) + blockSize
						);
				}
				else{
					//Filled White Box
					arduboy -> fillRect( 
						(borderSize + xPos) + revXHorz * blockSize, 
						(borderSize + yPos) + yVert * blockSize, 
						blockSize, blockSize, 
						WHITE);
				}
			}
		}
	}

	String debugOutput(){
		String toReturn = "size: ";
		toReturn += String(boardSize) + String("\n");
		return toReturn;
	}
	bool autoSize = true;
	uint8_t xPos = 64;
	uint8_t yPos = 0;
	uint8_t borderSize = 1;
	uint8_t blockSize = 4;
	uint8_t boardSize;
	uint16_t* board;
};

// void WrapValue(uint8_t* val, uint8_t max, bool inc) {
//     if (inc) {
//         *val = (*val + 1) % max;
//     } else {
//         *val = (*val == 0) ? (max - 1) : (*val - 1);
//     }
// }


class PicrossBoardManager : public Controllable, public Renderable {
public:
	PicrossBoardManager(Arduboy2* arduboy, ControllerList* inControllerList) 
	: Controllable(inControllerList), Renderable(arduboy) {}

	void setBoard(PicrossBoard* inBoard){
		currentBoard = inBoard;
	}

	void loadCurrentBoard( PicrossBoard inBoard ){
		currentBoard->setBoard(inBoard);
	}

	void loadCurrentBoard( SavedPicrossBoard inBoard){
		currentBoard->setBoard(inBoard);
	}

	void loadBoard(PicrossBoard* inBoard){
		if( completeBoard != NULL ){
			delete(completeBoard);
		}
		completeBoard = inBoard;

		if( currentBoard != NULL ){
			delete(currentBoard);
		}
		currentBoard = new PicrossBoard(arduboy, inBoard -> boardSize);
  		// Serial.println("Loading Board!");
	}

	void setRenderPos(uint8_t x, uint8_t y){
		xRenderPos = x;
		yRenderPos = y;
		if( currentBoard != NULL ){
			currentBoard -> setPos(x, y);
		}
	}

	static void MOVE_UP(void* data){
		PicrossBoardManager* pbm = static_cast<PicrossBoardManager*>(data);
		WrapValue( &(pbm -> yPos), pbm -> currentBoard -> boardSize, false);
	}
	static void MOVE_DOWN(void* data ){
		PicrossBoardManager* pbm = static_cast<PicrossBoardManager*>(data);
		WrapValue( &(pbm -> yPos), pbm -> currentBoard -> boardSize, true);
	}

	static void MOVE_LEFT(void* data){
		PicrossBoardManager* pbm = static_cast<PicrossBoardManager*>(data);
		WrapValue( &(pbm -> xPos), pbm -> currentBoard -> boardSize, false);
	}
	static void MOVE_RIGHT(void* data ){
		PicrossBoardManager* pbm = static_cast<PicrossBoardManager*>(data);
		WrapValue( &(pbm -> xPos), pbm -> currentBoard -> boardSize, true);
	}
	static void SWAP_BOARD(void* data){
		PicrossBoardManager* pbm = static_cast<PicrossBoardManager*>(data);

	    PicrossBoard* tempBoard = pbm->currentBoard;
	    pbm->currentBoard = pbm->completeBoard;
	    pbm->completeBoard = tempBoard;
	}

	static void FLIP_SELECTION(void* data){
		PicrossBoardManager* pbm = static_cast<PicrossBoardManager*>(data);
		pbm -> currentBoard -> toggleBitAt(pbm -> xPos, pbm -> yPos);
		
		//New Save System
		// SavedPicrossBoard save;
		// save.boardsize = pbm -> currentBoard -> boardSize;
		// for( int i = 0; i < save.boardsize; i++ ){
		// 	save.board[i] = pbm -> currentBoard -> boardSize;
		// }
		// SaveSystem::save(SAVED_BOARD, save);
		
	}

	void takeControl() override{
		addControl(BUTTON_JUST_PRESSED | BUTTON_FRAME_DELAYED_REPEATING, UP_BUTTON, &PicrossBoardManager::MOVE_UP, this);
		addControl(BUTTON_JUST_PRESSED | BUTTON_FRAME_DELAYED_REPEATING, DOWN_BUTTON, &PicrossBoardManager::MOVE_DOWN, this);
		addControl(BUTTON_JUST_PRESSED | BUTTON_FRAME_DELAYED_REPEATING, LEFT_BUTTON, &PicrossBoardManager::MOVE_LEFT, this);
		addControl(BUTTON_JUST_PRESSED | BUTTON_FRAME_DELAYED_REPEATING, RIGHT_BUTTON, &PicrossBoardManager::MOVE_RIGHT, this);
		
		// addControl(BUTTON_JUST_PRESSED, A_BUTTON, &PicrossBoardManager::SWAP_BOARD, this);
		addControl(BUTTON_JUST_PRESSED, B_BUTTON, &PicrossBoardManager::FLIP_SELECTION, this);
	}
	void loseControl() override{
		
	}
	String debugOutput(){
		String toReturn = currentBoard -> debugOutput();
		toReturn += String("X: ") + String(xPos) + String("\n");
		toReturn += String("Y: ") + String(yPos) + String("\n");
		for( int rowIndex = 0; rowIndex < currentBoard -> boardSize; rowIndex++ ){
			toReturn += String(currentBoard -> board[rowIndex]) + String(" : ") + String(completeBoard -> board[rowIndex]) + String("\n");
		}
		toReturn += String(currentBoard -> BoardEqual(completeBoard));
		return toReturn;
	}

	void render() override{
		currentBoard -> render();

		arduboy->setCursor(0, 0);
		arduboy->print(currentBoard -> boardSize);
		for(int i = 0; i < currentBoard->boardSize; i++ ){
			arduboy->setCursor(0, 8 + (i * 8));
			arduboy->print(currentBoard->board[i]);
		}

		//Draw Chisel
		int boardXPos = currentBoard -> getXPos();
		int boardYPos = currentBoard -> getYPos();
		int blockSize = currentBoard -> getAutoBlockSize();
		int borderSize = currentBoard -> getBorderSize();
		drawDottedRect( *arduboy,
			(borderSize + boardXPos) + xPos * blockSize, 
			(borderSize + boardYPos) + yPos * blockSize, 
			blockSize);
		// uint8_t markerColor = currentBoard -> getBitAt(xPos, yPos) ? WHITE : BLACK;
		// arduboy -> drawRect( 
		// 	(borderSize + boardXPos) + xPos * blockSize, 
		// 	(borderSize + boardYPos) + yPos * blockSize, 
		// 	blockSize, blockSize, 
		// 	markerColor);
	}

	bool boardComplete = false;
	PicrossBoard* currentBoard = NULL;
	PicrossBoard* completeBoard = NULL;
	uint8_t xPos = 0;
	uint8_t yPos = 0;
	uint8_t xRenderPos = 0;
	uint8_t yRenderPos = 0;
};

#endif