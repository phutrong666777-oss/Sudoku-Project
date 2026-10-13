#include "SudokuLogic.h"

// =========================================================================
// THỰC THI LỚP SudokuCell
// =========================================================================


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
    if (r < 0 || r >= 9 || c < 0 || c >= 9) return false;
    
    if (grid[r][c].isEditable() == false) return false;
    int currentValue = grid[r][c].getValue();

    if (currentValue == val) return true;

    // Lưu undo/redo...
    MoveRecord move = {r, c, currentValue, val};
    undoStack.push(move);
    while (!redoStack.empty()) redoStack.pop();

    // Gán giá trị người chơi nhập
    grid[r][c].setValue(val);

    // --- CẬP NHẬT TRẠNG THÁI ĐÚNG/SAI ---
    // So sánh giá trị nhập vào với đáp án gốc (Nếu giá trị != 0 và khác đáp án thì là sai)
    bool wrong = (val != 0 && val != solutionGrid[r][c]);
    grid[r][c].setIsWrong(wrong);

    // Trả về kết quả luật Sudoku
    return isValidMove(r, c, val);
}

// ==========================================================
// TRIỂN KHAI CÁC HÀM UNDO / REDO
// ==========================================================
bool SudokuBoard::undo() {
    if (undoStack.empty()) return false; // Không có gì để Undo

    // 1. Lấy thao tác cuối cùng ra với O(1)
    MoveRecord lastMove = undoStack.top();
    undoStack.pop();

    // 2. Chuyển nó sang ngăn xếp Redo
    redoStack.push(lastMove);

    // 3. Khôi phục lại giá trị cũ trên bảng (không dùng hàm setCell để tránh bị đẩy ngược lại vào stack)
    grid[lastMove.row][lastMove.col].setValue(lastMove.oldValue);

    return true; // Undo thành công
}

bool SudokuBoard::redo() {
    if (redoStack.empty()) return false; // Không có gì để Redo

    // 1. Lấy thao tác vừa bị Undo ra với O(1)
    MoveRecord moveToRedo = redoStack.top();
    redoStack.pop();

    // 2. Đẩy ngược lại vào ngăn xếp Undo
    undoStack.push(moveToRedo);

    // 3. Áp dụng lại giá trị mới
    grid[moveToRedo.row][moveToRedo.col].setValue(moveToRedo.newValue);

    return true; // Redo thành công
}

void SudokuBoard::clearHistory() {
    while(!undoStack.empty()) undoStack.pop();
    while(!redoStack.empty()) redoStack.pop();
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
    for(int r = 0; r < 9; ++r)
        for(int c = 0; c < 9; ++c)
            grid[r][c] = SudokuCell(r, c, 0);

    std::vector<int> firstRow = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(firstRow.begin(), firstRow.end(), g);

    for(int c = 0; c < 9; ++c) {
        grid[0][c].setValue(firstRow[c]);
    }
    
    solve();
    
    // Đánh dấu tất cả ô hiện tại là ô cố định (isFixed = true)
    for(int r = 0; r < 9; ++r) {
        for(int c = 0; c < 9; ++c) {
            grid[r][c].setFixed(true);
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
    clearHistory();
    currentDifficulty = diff;
    generateSolvedBoard();

    for(int r = 0; r < 9; ++r) {
        for(int c = 0; c < 9; ++c) {
            solutionGrid[r][c] = grid[r][c].getValue(); // Lưu lại bảng đã giải để dùng cho hint
            cout<<"[SudokuBoard] SolutionGrid["<<r<<"]["<<c<<"] = "<<solutionGrid[r][c]<<"\n";
        }
    }
    
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

    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            data += std::to_string(solutionGrid[r][c]);
        }
    }
    return data;
}

void SudokuBoard::loadBoardData(const string& data) {
    clearHistory();
    if (data.length() != 162) return; // Lỗi data
    
    int index = 0;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            int val = data[index] - '0'; // Chuyển char thành int
            grid[r][c] = SudokuCell(r, c, val);
            //cout<<grid[r][c].getValue()<<"\n";
            grid[r][c].setFixed(val != 0);
            index++;
        }
    }

    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            int val = data[index] - '0'; // Chuyển char thành int
            solutionGrid[r][c] = val;
            index++;
        }
    }
}

bool SudokuBoard::isGiveHint(int& hintRow, int& hintCol) {
    if(grid[hintRow][hintCol].getValue() == 0) {
        grid[hintRow][hintCol].setValue(solutionGrid[hintRow][hintCol]);
        return true;
    }
    cout << "[SudokuBoard] Please select a valid cell for a hint<<\n";
    return false;
}




