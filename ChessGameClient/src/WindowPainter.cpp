#include "WindowPainter.hpp"
#include <algorithm>

void WindowPainter::LoadSprites() {
	this->LoadBoardSprite(L"GameField.jpg");
	this->LoadFigureSprites(L"ChessFigures.png");
}

void WindowPainter::SetBoard(Board* board)
{
	this->board = board;
}

void WindowPainter::LoadFigureSprites( std::wstring folderPath) {
	board->figureSprites = Bitmap::FromFile(folderPath.c_str());

	RectF srcRect;
	Unit srcUnit;
	board->figureSprites->GetBounds(&srcRect, &srcUnit);

	int figureWidth = srcRect.Width / 6.0;
	int figureHeight = srcRect.Height / 2.0;
	board->figureInfo.width = board->boardImageInfo.cellWidth;
	board->figureInfo.height = board->boardImageInfo.cellHeight;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++) {
			Rect rect(j * figureWidth, i * figureHeight, figureWidth, figureHeight);
			board->figuresBoundingRect[i*6 + j] = rect;
		}
	}
}

void WindowPainter::LoadBoardSprite( std::wstring path) {
	//To DO: разные поля с разными характеристиками
	board->sprite = Image::FromFile(path.c_str());

	board->boardImageInfo.width = board->sprite->GetWidth();
	board->boardImageInfo.height = board->sprite->GetHeight();
	board->boardImageInfo.topOffset = 18;
	board->boardImageInfo.leftOffset = 19;
	board->boardImageInfo.bottomOffset = 21;
	board->boardImageInfo.rightOffest = 20;
	board->boardImageInfo.cellWidth = 95;
	board->boardImageInfo.cellHeight = 95;

	Bitmap tmpBitmap(path.c_str());
	tmpBitmap.GetPixel(1, 1, &board->boardImageInfo.borderColor);
	board->darkBoardColor = Color(board->boardImageInfo.borderColor.GetA(), board->boardImageInfo.borderColor.GetR(), board->boardImageInfo.borderColor.GetG(), board->boardImageInfo.borderColor.GetB());
	board->lightBoardColor = Color(255, 208, 191, 163);
}

Button* WindowPainter::CreateButton(Rect rect, std::wstring text, bool isShown, int id)
{
	Button button;
	button.id = id;
	button.rect = rect;
	button.text = text;
	button.isShown = isShown;
	buttons.push_back(button);
	return &buttons[buttons.size() - 1];
}

Panel* WindowPainter::CreatePanel(Rect rect, std::vector<std::wstring> texts, std::vector<Color> textsColor, bool isShown, int id, bool dark)
{
	Panel panel;
	panel.id = id;
	panel.rect = rect;
	panel.texts = texts;
	panel.textsColor = textsColor;
	panel.isShown = isShown;
	panel.dark = dark;
	panels.push_back(panel);
	return &panels[panels.size() - 1];
}

void WindowPainter::ChangeButtonVisibility(int id)
{
	for (int i = 0; i < buttons.size(); i++) {
		if (buttons[i].id == id) {
			buttons[i].isShown = !buttons[i].isShown;
			break;
		}
	}
	return;
}

void WindowPainter::ChangePanelVisibility(int id)
{
	for (int i = 0; i < panels.size(); i++) {
		if (panels[i].id == id) {
			panels[i].isShown = !panels[i].isShown;
			break;
		}
	}
	return;
}

void WindowPainter::ChangeAllControlsVisibility()
{
	for (int i = 0; i < buttons.size(); i++) {
		buttons[i].isShown = !buttons[i].isShown;
	}
	for (int i = 0; i < panels.size(); i++) {
		panels[i].isShown = !panels[i].isShown;
	}
}

void WindowPainter::DrawControls()
{
	this->DrawButtons();
	this->DrawPanels();
}

void WindowPainter::DrawButtons()
{
	SolidBrush brush(Color(255, 208, 191, 163));
	SolidBrush textSolidBrush(Color(255, 0, 0, 0));
	Pen pen(Color(255, 0, 0, 0), 2);

	SolidBrush pushedBrush(Color(255, 218, 201, 173));
	SolidBrush pushedTextSolidBrush(Color(255, 0, 0, 0));
	Pen pushedpen(Color(255, 0, 0, 0), 2);

	SolidBrush clickedBrush(Color(255, 238, 221, 193));
	SolidBrush clickedTextSolidBrush(Color(255, 0, 0, 0));
	Pen clickedpen(Color(255, 0, 0, 0), 2);

	FontFamily   fontFamily(L"Courier New");
	Font         font(&fontFamily, 12, FontStyleBold, UnitPoint);
	StringFormat stringFormat;	
	
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetLineAlignment(StringAlignmentCenter);

	for (int i = 0; i < buttons.size(); i++) {
		if (!buttons[i].isShown) {
			continue;
		}

		RectF rectF(buttons[i].rect.X, buttons[i].rect.Y, buttons[i].rect.Width, buttons[i].rect.Height);
		if (buttons[i].rect.IntersectsWith(Rect(clickedXMousePos, clickedYMousePos, 1, 1))) {
			rectF.X += 2; rectF.Y += 2;
			this->currentGraphics->FillRectangle(&clickedBrush, buttons[i].rect);
			this->currentGraphics->DrawRectangle(&clickedpen, Rect(buttons[i].rect.X + 1, buttons[i].rect.Y, buttons[i].rect.Width - 1, buttons[i].rect.Height));
			this->currentGraphics->DrawString(buttons[i].text.c_str(), -1, &font, rectF, &stringFormat, &clickedTextSolidBrush);
		}
		else if (intersectedRect == &buttons[i].rect) {
			this->currentGraphics->FillRectangle(&pushedBrush, buttons[i].rect);
			this->currentGraphics->DrawRectangle(&pushedpen, Rect(buttons[i].rect.X + 1, buttons[i].rect.Y, buttons[i].rect.Width - 1, buttons[i].rect.Height));
			this->currentGraphics->DrawString(buttons[i].text.c_str(), -1, &font, rectF, &stringFormat, &pushedTextSolidBrush);
		}
		else {			
			this->currentGraphics->FillRectangle(&brush, buttons[i].rect);
			this->currentGraphics->DrawRectangle(&pen, Rect(buttons[i].rect.X + 1, buttons[i].rect.Y, buttons[i].rect.Width - 1, buttons[i].rect.Height));
			this->currentGraphics->DrawString(buttons[i].text.c_str(), -1, &font, rectF, &stringFormat, &textSolidBrush);
		}

	}
}

void WindowPainter::DrawPanels()
{
	Pen pen(Color(255, 0, 0, 0), 2);
	FontFamily   fontFamily(L"Courier New");
	StringFormat stringFormat;

	for (int i = 0; i < panels.size(); i++) {
		if (!panels[i].isShown) {
			continue;
		}
		if (panels[i].texts.size() == 1) {
			stringFormat.SetAlignment(StringAlignmentCenter);
			stringFormat.SetLineAlignment(StringAlignmentCenter);
		}
		else {
			stringFormat.SetAlignment(StringAlignmentNear);
			stringFormat.SetLineAlignment(StringAlignmentNear);
		}	

		SolidBrush brush(panels[i].dark ? board->darkBoardColor : board->lightBoardColor);		
		Pen pen2(!panels[i].dark ? board->darkBoardColor : board->lightBoardColor, 2);
		this->currentGraphics->FillRectangle(&brush, panels[i].rect);
		this->currentGraphics->DrawRectangle(&pen, Rect(panels[i].rect.X + 1, panels[i].rect.Y, panels[i].rect.Width - 1, panels[i].rect.Height));
	//	this->currentGraphics->DrawRectangle(&pen2, Rect(panels[i].rect.X + 2, panels[i].rect.Y + 2, panels[i].rect.Width - 3, panels[i].rect.Height - 4));

		for (int j = 0; j < panels[i].texts.size(); j++) {
			RectF rectF(panels[i].rect.X, panels[i].rect.Y + panels[i].rect.Height / panels[i].texts.size() * j, panels[i].rect.Width, panels[i].rect.Height / panels[i].texts.size());
			Font  font(&fontFamily, panels[i].rect.Width / (panels[i].texts[i].size() - 2), FontStyleBold, UnitPoint);
			SolidBrush textSolidBrush(panels[i].textsColor[j]);
			this->currentGraphics->DrawString(panels[i].texts[j].c_str(), -1, &font, rectF, &stringFormat, &textSolidBrush);
		}
		
	}
}

void WindowPainter::DrawMenu()
{
	SolidBrush brush(this->board->darkBoardColor); 
	Pen pen(this->board->lightBoardColor, 2);

	Rect menuRect(this->board->boardInfo.rect.X + this->board->boardInfo.rect.Width,
		this->windowRect.top,
		this->windowRect.right - this->windowRect.left - (this->board->boardInfo.rect.X + this->board->boardInfo.rect.Width),
		this->windowRect.bottom - this->windowRect.top);
	this->currentGraphics->FillRectangle(&brush, menuRect);
	this->currentGraphics->DrawRectangle(&pen, menuRect);
}

int WindowPainter::ClickButton()
{
	for (int i = 0; i < buttons.size(); i++) {
		if (!buttons[i].isShown) {
			continue;
		}
		else if (intersectedRect == &buttons[i].rect) {
			return buttons[i].id;
		}
	}
	return 0;
}

bool WindowPainter::NeedInvalidation()
{
	for (int i = 0; i < buttons.size(); i++) {
		if (!buttons[i].isShown) {
			continue;
		}
		if (buttons[i].rect.IntersectsWith(Rect(xMousePos, yMousePos, 1, 1))) {
			intersectedRect = &buttons[i].rect;
			return true;
		}
		else if (intersectedRect == &buttons[i].rect) {
			intersectedRect = nullptr;
			return true;
		}
	}
	return false;
}

void WindowPainter::DrawField() {
	SolidBrush whiteBrush(Color(255,255,255,255));
	SolidBrush fieldBorderBrush(board->darkBoardColor);
	Pen pen(this->board->lightBoardColor, 2);
	this->currentGraphics->FillRectangle(&whiteBrush, 0, 0, this->windowRect.right - this->windowRect.left, this->windowRect.bottom - this->windowRect.top);
	this->currentGraphics->FillRectangle(&fieldBorderBrush, 0, 0, board->boardInfo.rect.Width, this->windowRect.bottom - this->windowRect.top);	
	this->currentGraphics->DrawImage(board->sprite, board->boardInfo.rect);
//	this->currentGraphics->DrawRectangle(&pen, 1, 1, board->boardInfo.rect.Width - 1, this->windowRect.bottom - this->windowRect.top - 1);
}

void WindowPainter::DrawFigures() {
	for (int i = 0; i < board->figures.size(); i++) {
		for (int j = 0; j < 8; j++) {
			if (board->figures[i][j] != nullptr && board->figures[i][j] != board->selectedFigure) {
				float x, y;
				x = board->boardInfo.rect.X + (board->boardImageInfo.topOffset + i * board->boardImageInfo.cellWidth + board->boardImageInfo.cellWidth / 2.0f - board->figureInfo.width / 2.0f) * board->boardInfo.boardSizeMult;
				y = board->boardInfo.rect.Y + (board->boardImageInfo.leftOffset + j * board->boardImageInfo.cellHeight + board->boardImageInfo.cellHeight / 2.0f - board->figureInfo.height / 2.0f) * board->boardInfo.boardSizeMult;
				RectF destRect(x, y, board->figureInfo.width * board->boardInfo.boardSizeMult, board->figureInfo.height * board->boardInfo.boardSizeMult);
				Rect srcRect = board->figuresBoundingRect[board->figures[i][j]->type + board->figures[i][j]->side * 6];
				this->currentGraphics->DrawImage(board->figureSprites, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel);
			}
		}
	}	
}

void WindowPainter::DrawSelectedFigure()
{
	if (board->selectedFigure != nullptr) {
		float x, y;
		x = xMousePos - (board->figureInfo.width / 2.0f) * board->boardInfo.boardSizeMult;
		y = yMousePos - (board->figureInfo.height / 2.0f) * board->boardInfo.boardSizeMult;
		RectF destRect(x, y, board->figureInfo.width * board->boardInfo.boardSizeMult, board->figureInfo.height * board->boardInfo.boardSizeMult);
		Rect srcRect = board->figuresBoundingRect[board->selectedFigure->type + board->selectedFigure->side * 6];
		this->currentGraphics->DrawImage(board->figureSprites, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel);
	}	
}

void WindowPainter::DrawHintMoves()
{
	if (board->selectedFigure != nullptr) {

		Pen hintPen(Color(50, 50, 205, 50), 1);
		SolidBrush hintBrush(Color(50, 50, 205, 50));

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (board->selectedFigure->possibleMovesMap[i][j]) {
					float x, y, height, width;
					x = board->boardInfo.rect.X + (board->boardImageInfo.topOffset + i * board->boardImageInfo.cellWidth) * board->boardInfo.boardSizeMult;
					y = board->boardInfo.rect.Y + (board->boardImageInfo.leftOffset + j * board->boardImageInfo.cellHeight) * board->boardInfo.boardSizeMult;
					width = board->boardImageInfo.cellWidth * board->boardInfo.boardSizeMult;
					height = board->boardImageInfo.cellHeight * board->boardInfo.boardSizeMult + 1;

					this->currentGraphics->DrawRectangle(&hintPen, x + 1, y - 1, width, height);
					this->currentGraphics->FillRectangle(&hintBrush, x + 1, y - 1, width, height);
				}				
			}
		}		
	}
}

void WindowPainter::DrawCurrentMoveCell()
{
	if (board->selectedFigure) {
		Point cellCoords = board->selectCell(Point(xMousePos, yMousePos));
		if (!(cellCoords.X > 7 || cellCoords.X < 0 || cellCoords.Y > 7 || cellCoords.Y < 0)) {
			if (board->selectedFigure->possibleMovesMap[cellCoords.X][cellCoords.Y]) {
				float x = board->boardInfo.rect.X + (board->boardImageInfo.topOffset + cellCoords.X * board->boardImageInfo.cellWidth) * board->boardInfo.boardSizeMult;
				float y = board->boardInfo.rect.Y + (board->boardImageInfo.leftOffset + cellCoords.Y * board->boardImageInfo.cellHeight) * board->boardInfo.boardSizeMult;
				float width = board->boardImageInfo.cellWidth * board->boardInfo.boardSizeMult;
				float height = board->boardImageInfo.cellHeight * board->boardInfo.boardSizeMult + 1;

				Pen selectedCellHintPen(Color(100, 255, 255, 0), width / 20);

				this->currentGraphics->DrawRectangle(&selectedCellHintPen, x + 1, y - 1, width, height);
			}
		}	
	}	
}

void WindowPainter::DrawDeadFigures()
{
	int offset = 2;

	std::vector<Figure*> sortedWhite;
	std::vector<Figure*> sortedBlack;
	for (int i = 0; i < board->deadFigures.size(); i++) {
		if (board->deadFigures[i]->side) {
			sortedWhite.push_back(board->deadFigures[i]);
		}
		else {
			sortedBlack.push_back(board->deadFigures[i]);
		}
	}
	std::sort(sortedBlack.begin(), sortedBlack.end(), [](Figure* a, Figure* b) {
		return a->type > b->type;
	});
	std::sort(sortedWhite.begin(), sortedWhite.end(), [](Figure* a, Figure* b) {
		return a->type > b->type;
	});

	for (int i = 0; i < sortedWhite.size(); i++) {
		float x, y;
		Figure* deadFigure = sortedWhite[i];
		Rect deadFigureRect = deadWhiteFiguresRect;

		x = deadFigureRect.X + (deadFigureRect.Height - offset) * i / 2;
		y = deadFigureRect.Y + offset/2;
		RectF destRect(x, y, (deadFigureRect.Height - offset), (deadFigureRect.Height - offset));

		Rect srcRect = board->figuresBoundingRect[sortedWhite[i]->type + sortedWhite[i]->side * 6];
		this->currentGraphics->DrawImage(board->figureSprites, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel);
	}
	for (int i = 0; i < sortedBlack.size(); i++) {
		float x, y;
		Figure* deadFigure = sortedBlack[i];
		Rect deadFigureRect = deadBlackFiguresRect;

		x = deadFigureRect.X + (deadFigureRect.Height - offset) * i / 2;
		y = deadFigureRect.Y + offset / 2;
		RectF destRect(x, y, (deadFigureRect.Height - offset), (deadFigureRect.Height - offset));

		Rect srcRect = board->figuresBoundingRect[sortedBlack[i]->type + sortedBlack[i]->side * 6];
		this->currentGraphics->DrawImage(board->figureSprites, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel);
	}
}

void WindowPainter::DrawDangerHints( int currentSide)
{
	if (!board->figuresAttackingKing.empty()) {
		float width = board->boardImageInfo.cellWidth * board->boardInfo.boardSizeMult;
		float height = board->boardImageInfo.cellHeight * board->boardInfo.boardSizeMult + 1;

		Pen kDangerHintPen(Color(100, 255, 0, 0), width / 20);

		bool kingFound = false;
		for (int i = 0; i < 8 && !kingFound; i++) {
			for (int j = 0; j < 8 && !kingFound; j++) {
				if (board->figures[i][j] && board->figures[i][j]->side == currentSide && board->figures[i][j]->type == FigureType::King) {
					float x = board->boardInfo.rect.X + (board->boardImageInfo.topOffset + i * board->boardImageInfo.cellWidth) * board->boardInfo.boardSizeMult;
					float y = board->boardInfo.rect.Y + (board->boardImageInfo.leftOffset + j * board->boardImageInfo.cellHeight) * board->boardInfo.boardSizeMult;

					this->currentGraphics->DrawRectangle(&kDangerHintPen, x + 1, y - 1, width, height);
					//this->currentGraphics->FillRectangle(&kDangerHintBrush, x + 1, y - 1, width, height);

					kingFound = true;					
				}
			}
		}

		for (int i = 0; i < board->figuresAttackingKing.size(); i++) {
			float x = board->boardInfo.rect.X + (board->boardImageInfo.topOffset + board->figuresAttackingKing[i].X * board->boardImageInfo.cellWidth) * board->boardInfo.boardSizeMult;
			float y = board->boardInfo.rect.Y + (board->boardImageInfo.leftOffset + board->figuresAttackingKing[i].Y * board->boardImageInfo.cellHeight) * board->boardInfo.boardSizeMult;

			this->currentGraphics->DrawRectangle(&kDangerHintPen, x + 1 , y - 1, width, height);
			//this->currentGraphics->FillRectangle(&dangerHintBrush, x + 1, y - 1, width, height);
		}
	}	
}

void WindowPainter::DrawLoggerWindow(Rect rect)
{
	/*Pen hintPen(Color(50, 50, 50, 50), 1);
	SolidBrush hintBrush(Color(50, 50, 50, 50));

	this->currentGraphics->FillRectangle(&hintBrush, rect.X, rect.Y, rect.Width, rect.Height);*/
}

void WindowPainter::CreateBuffer(HWND hwnd) {
	HDC tmpHdc = GetWindowDC(hwnd);
	this->bufferDC = CreateCompatibleDC(tmpHdc);
	this->memoryDC = CreateCompatibleDC(tmpHdc);

	HBITMAP hBitmap = CreateCompatibleBitmap(tmpHdc, this->windowRect.right - this->windowRect.left, this->windowRect.bottom - this->windowRect.top);
	SelectObject(bufferDC, hBitmap);
	DeleteObject(hBitmap);

	hBitmap = CreateCompatibleBitmap(tmpHdc, this->windowRect.right - this->windowRect.left, this->windowRect.bottom - this->windowRect.top);
	SelectObject(memoryDC, hBitmap);
	DeleteObject(hBitmap);

	ReleaseDC(hwnd, tmpHdc);
}

void WindowPainter::SetHDC(HDC hdc) {
	if (this->currentGraphics != nullptr)
		this->currentGraphics->~Graphics();
	this->currentHDC = hdc;
	this->currentGraphics = Gdiplus::Graphics::FromHDC(hdc);
}

void WindowPainter::SetWindow(HWND hwnd, INT bottomMargin, INT topMargin) {
	GetClientRect(hwnd, &this->windowRect);

	board->boardInfo.rect.Height = this->windowRect.bottom - this->windowRect.top - bottomMargin - topMargin;
	board->boardInfo.rect.Width = (board->boardImageInfo.width / (board->boardImageInfo.height * 1.0)) * board->boardInfo.rect.Height;
	board->boardInfo.boardSizeMult = (1.0f * board->boardInfo.rect.Height) / (board->boardImageInfo.height);
	board->boardInfo.rect.X = 0;
	board->boardInfo.rect.Y = topMargin;

	board->boardInfo.tagRect.top = topMargin;
	board->boardInfo.tagRect.left = 0;
	board->boardInfo.tagRect.right = board->boardInfo.tagRect.left + board->boardInfo.rect.Width;
	board->boardInfo.tagRect.bottom = board->boardInfo.tagRect.top + board->boardInfo.rect.Height;

	ClientToScreen(hwnd, reinterpret_cast<POINT*>(&board->boardInfo.tagRect.left)); // convert top-left
	ClientToScreen(hwnd, reinterpret_cast<POINT*>(&board->boardInfo.tagRect.right)); // convert bottom-right
}
