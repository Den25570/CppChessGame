#include "WindowPainter.hpp"

void WindowPainter::LoadSprites() {
	this->LoadBoardSprite(L"\\GameField.jpg");
}

void WindowPainter::LoadBoardSprite(std::wstring path) {
	//To DO: разные поля с разными характеристиками
	this->game.board.sprite = Image::FromFile(path.c_str());

	this->game.board.boardImageInfo.width = 800;
	this->game.board.boardImageInfo.height = 800;
	this->game.board.boardImageInfo.topOffset = 18;
	this->game.board.boardImageInfo.leftOffset = 19;
	this->game.board.boardImageInfo.bottomOffset = 21;
	this->game.board.boardImageInfo.rightOffest = 20;
	this->game.board.boardImageInfo.cellWidth = 95;
	this->game.board.boardImageInfo.cellHeight = 95;
}

void WindowPainter::DrawField() {
	INT height = this->windowRect.bottom - this->windowRect.top;
	INT width = (this->game.board.sprite->GetWidth() / (this->game.board.sprite->GetHeight() * 1.0)) * height;
	this->game.board.boardInfo.boardSizeMult = (1.0f * height) / (this->game.board.boardImageInfo.height);

	this->game.board.boardInfo.xPos = 0;
	this->game.board.boardInfo.yPos = 0;

	this->currentGraphics->DrawImage(this->game.board.sprite, this->game.board.boardInfo.xPos, this->game.board.boardInfo.yPos, width, height);
}

void WindowPainter::DrawFigures() {
	Pen bluePen(Color(255, 0, 0, 255));
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			float x = (this->game.board.boardImageInfo.topOffset + i * this->game.board.boardImageInfo.cellWidth + this->game.board.boardImageInfo.cellWidth / 2.0f) * this->game.board.boardInfo.boardSizeMult;
			float y = (this->game.board.boardImageInfo.leftOffset + j * this->game.board.boardImageInfo.cellHeight + this->game.board.boardImageInfo.cellHeight / 2.0f) * this->game.board.boardInfo.boardSizeMult;

			RectF ellipseRect(x, y, 40.0f, 40.0f);
			this->currentGraphics->DrawEllipse(&bluePen, ellipseRect);
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

void WindowPainter::FreeResources() {
	if (this->currentGraphics != nullptr)
		this->currentGraphics->~Graphics();
	if (this->game.board.sprite != nullptr)
		this->game.board.sprite->~Image();
}
