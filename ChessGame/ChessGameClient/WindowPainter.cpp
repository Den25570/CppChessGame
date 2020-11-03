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
	this->currentGraphics->DrawImage(board->sprite, board->boardInfo.rect);
}

void WindowPainter::DrawFigures(Board* board) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board->figures[i][j] != nullptr && board->figures[i][j] != board->selectedFigure) {
				float x, y;
				x = (board->boardImageInfo.topOffset + i * board->boardImageInfo.cellWidth + board->boardImageInfo.cellWidth / 2.0f - board->figureInfo.width / 2.0f) * board->boardInfo.boardSizeMult;
				y = (board->boardImageInfo.leftOffset + j * board->boardImageInfo.cellHeight + board->boardImageInfo.cellHeight / 2.0f - board->figureInfo.height / 2.0f) * board->boardInfo.boardSizeMult;
				RectF destRect(x, y, board->figureInfo.width * board->boardInfo.boardSizeMult, board->figureInfo.height * board->boardInfo.boardSizeMult);
				Rect srcRect = board->figuresBoundingRect[board->figures[i][j]->type + board->figures[i][j]->side * 6];
				this->currentGraphics->DrawImage(board->figureSprites, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel);
			}
		}
	}	
}

void WindowPainter::DrawSelectedFigure(Board* board)
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

void WindowPainter::DrawHintMoves(Board* board)
{
	if (board->selectedFigure != nullptr) {

		Pen hintPen(Color(50, 50, 205, 50), 1);
		SolidBrush hintBrush(Color(50, 50, 205, 50));

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (board->selectedFigure->possibleMovesMap[i][j]) {
					float x, y, height, width;
					x = (board->boardImageInfo.topOffset + i * board->boardImageInfo.cellWidth) * board->boardInfo.boardSizeMult + 1;
					y = (board->boardImageInfo.leftOffset + j * board->boardImageInfo.cellHeight) * board->boardInfo.boardSizeMult - 1;
					width = board->boardImageInfo.cellWidth * board->boardInfo.boardSizeMult;
					height = board->boardImageInfo.cellHeight * board->boardInfo.boardSizeMult;

					this->currentGraphics->DrawRectangle(&hintPen, x, y, width, height);
					this->currentGraphics->FillRectangle(&hintBrush, x + 1, y + 1, width, height);
				}				
			}
		}		
	}
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

void WindowPainter::SetWindow(HWND hwnd, Board * board) {
	GetClientRect(hwnd, &this->windowRect);

	board->boardInfo.rect.Height = this->windowRect.bottom - this->windowRect.top;
	board->boardInfo.rect.Width = (board->boardImageInfo.width / (board->boardImageInfo.height * 1.0)) * board->boardInfo.rect.Height;
	board->boardInfo.boardSizeMult = (1.0f * board->boardInfo.rect.Height) / (board->boardImageInfo.height);
	board->boardInfo.rect.X = 0;
	board->boardInfo.rect.Y = 0;

	board->boardInfo.tagRect.top = 0;
	board->boardInfo.tagRect.left = 0;
	board->boardInfo.tagRect.right = board->boardInfo.tagRect.left + board->boardInfo.rect.Width;
	board->boardInfo.tagRect.bottom = board->boardInfo.tagRect.top + board->boardInfo.rect.Height;

	ClientToScreen(hwnd, reinterpret_cast<POINT*>(&board->boardInfo.tagRect.left)); // convert top-left
	ClientToScreen(hwnd, reinterpret_cast<POINT*>(&board->boardInfo.tagRect.right)); // convert bottom-right
}
