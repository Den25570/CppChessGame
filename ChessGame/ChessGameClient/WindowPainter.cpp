#include "WindowPainter.hpp"

void WindowPainter::LoadSprites(Board* board) {
	this->LoadBoardSprite(board, L"GameField.jpg");
	this->LoadFigureSprites(board, L"ChessFigures.png");
}

void WindowPainter::LoadFigureSprites(Board* board, std::wstring folderPath) {
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

void WindowPainter::LoadBoardSprite(Board* board, std::wstring path) {
	//To DO: разные поля с разными характеристиками
	board->sprite = Image::FromFile(path.c_str());

	board->boardImageInfo.width = 800;
	board->boardImageInfo.height = 800;
	board->boardImageInfo.topOffset = 18;
	board->boardImageInfo.leftOffset = 19;
	board->boardImageInfo.bottomOffset = 21;
	board->boardImageInfo.rightOffest = 20;
	board->boardImageInfo.cellWidth = 95;
	board->boardImageInfo.cellHeight = 95;
}

void WindowPainter::DrawField(Board* board) {
	INT height = this->windowRect.bottom - this->windowRect.top;
	INT width = (board->sprite->GetWidth() / (board->sprite->GetHeight() * 1.0)) * height;
	board->boardInfo.boardSizeMult = (1.0f * height) / (board->boardImageInfo.height);
	board->boardInfo.xPos = 0;
	board->boardInfo.yPos = 0;

	this->currentGraphics->DrawImage(board->sprite, board->boardInfo.xPos, board->boardInfo.yPos, width, height);
}

void WindowPainter::DrawFigures(Board* board) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			float x = (board->boardImageInfo.topOffset + i * board->boardImageInfo.cellWidth + board->boardImageInfo.cellWidth / 2.0f - board->figureInfo.width / 2.0f) * board->boardInfo.boardSizeMult;
			float y = (board->boardImageInfo.leftOffset + j * board->boardImageInfo.cellHeight + board->boardImageInfo.cellHeight / 2.0f - board->figureInfo.height / 2.0f) * board->boardInfo.boardSizeMult;

			RectF destRect(x, y, board->figureInfo.width * board->boardInfo.boardSizeMult, board->figureInfo.height * board->boardInfo.boardSizeMult);
			if (board->figures[i][j] != nullptr) {
				Rect srcRect = board->figuresBoundingRect[board->figures[i][j]->type + board->figures[i][j]->side * 6];
				this->currentGraphics->DrawImage(board->figureSprites, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel);
			}
		}
	}
	
}

void WindowPainter::SetHDC(HDC hdc) {
	if (this->currentGraphics != nullptr)
		this->currentGraphics->~Graphics();
	this->currentHDC = hdc;
	this->currentGraphics = Gdiplus::Graphics::FromHDC(hdc);
}

void WindowPainter::SetWindow(HWND hwnd) {
	GetClientRect(hwnd, &this->windowRect);
}
