#include "SudokuLogic.h"

// =========================================================================
// THỰC THI LỚP SudokuCell
// =========================================================================

void SudokuCell::update() {
    // Với một ô Sudoku cơ bản, thường không có logic update liên tục mỗi frame.
    // Hàm này để trống hoặc dùng để xử lý hiệu ứng nhấp nháy (nếu có sau này).
}

void SudokuCell::draw() {
    // Để trống. Vỹ An & Đăng Khôi sẽ implement nội dung render SFML sau.
}


// =========================================================================
// THỰC THI LỚP SudokuBoard
// =========================================================================

SudokuBoard::SudokuBoard() : currentDifficulty(Difficulty::MEDIUM) {
    srand(time(NULL)); // Khởi tạo seed random
    
    // Tạo bảng 9x9 với các ô trống (giá trị 0)
    for (int r = 0; r < 9; ++r) {
        std::vector<SudokuCell> row;
        for (int c = 0; c < 9; ++c) {
            row.push_back(SudokuCell(r, c, 0));
        }
        grid.push_back(row);
    }
}

bool SudokuBoard::isValidMove(int r, int c, int val) {
    // Kiểm tra hàng và cột
    for (int i = 0; i < 9; ++i) {
        if (grid[r][i].getValue() == val && i != c) return false;
        if (grid[i][c].getValue() == val && i != r) return false;
    }
    
    // Kiểm tra block 3x3
    int startRow = r - (r % 3);
    int startCol = c - (c % 3);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            int checkRow = startRow + i;
            int checkCol = startCol + j;
            if (grid[checkRow][checkCol].getValue() == val && (checkRow != r || checkCol != c)) {
                return false;
            }
        }
    }
    return true;
}

bool SudokuBoard::setCell(int r, int c, int val) {
    if (grid[r][c].isEditable()) {
        // Cho phép đặt giá trị 0 (tức là xóa ô) hoặc giá trị hợp lệ 1-9
        if (val == 0 || isValidMove(r, c, val)) {
            grid[r][c].setValue(val);
            return true;
        }
    }
    return false;
}

bool SudokuBoard::checkWin() {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            int val = grid[r][c].getValue();
            if (val == 0 || !isValidMove(r, c, val)) {
                return false; // Còn ô trống hoặc có ô sai luật
            }
        }
    }
    return true;
}

bool SudokuBoard::solve() {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (grid[r][c].getValue() == 0) {
                for (int val = 1; val <= 9; ++val) {
                    if (isValidMove(r, c, val)) {
                        grid[r][c].setValue(val);
                        if (solve()) return true;
                        grid[r][c].setValue(0); // Backtrack
                    }
                }
                return false; // Không tìm được số nào hợp lệ
            }
        }
    }
    return true; // Đã điền kín bảng
}

void SudokuBoard::generateSolvedBoard() {
    // Xóa sạch bảng
    for(int r=0; r<9; ++r)
        for(int c=0; c<9; ++c)
            grid[r][c] = SudokuCell(r, c, 0);

    // Thuật toán điền ngẫu nhiên dòng đầu tiên để tạo map đa dạng, sau đó solve
    for(int c=0; c<9; ++c) {
        int randVal = (rand() % 9) + 1;
        while(!isValidMove(0, c, randVal)) {
            randVal = (rand() % 9) + 1;
        }
        grid[0][c].setValue(randVal);
    }
    solve();
    
    // Đánh dấu toàn bộ là cố định (isFixed)
    for(int r=0; r<9; ++r) {
        for(int c=0; c<9; ++c) {
            grid[r][c] = SudokuCell(r, c, grid[r][c].getValue());
        }
    }
}

void SudokuBoard::removeCells(int numCellsToRemove) {
    int count = 0;
    while (count < numCellsToRemove) {
        int r = rand() % 9;
        int c = rand() % 9;
        if (grid[r][c].getValue() != 0) {
            grid[r][c] = SudokuCell(r, c, 0); // Đổi thành ô trống (có thể edit)
            count++;
        }
    }
}

void SudokuBoard::generatePuzzle(Difficulty diff) {
    currentDifficulty = diff;
    generateSolvedBoard();
    
    int cellsToRemove = 40; // Mặc định MEDIUM
    if (diff == Difficulty::EASY) cellsToRemove = 30;
    else if (diff == Difficulty::HARD) cellsToRemove = 50;
    else if (diff == Difficulty::EXPERT) cellsToRemove = 60;
    
    removeCells(cellsToRemove);
}

string SudokuBoard::serializeBoardData() {
    // Chuyển 81 ô thành 1 chuỗi dài 81 ký tự để lưu file. 
    // Format: '0' -> ô trống, '1'-'9' -> có số.
    string data = "";
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            data += std::to_string(grid[r][c].getValue());
        }
    }
    return data;
}

void SudokuBoard::loadBoardData(const string& data) {
    if (data.length() != 81) return; // Lỗi data
    
    int index = 0;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            int val = data[index] - '0'; // Chuyển char thành int
            grid[r][c] = SudokuCell(r, c, val);
            index++;
        }
    }
}

void SudokuBoard::update() {
    // Để trống hoặc gọi update của từng ô
}

void SudokuBoard::draw() {
    // Để trống. SFML Team sẽ lo.
}