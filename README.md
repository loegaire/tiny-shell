# thinsh (tiny shell + ngôn ngữ kịch bản "trang")

## Tính năng shell tương tác
- Chạy lệnh Linux thường (date, time, ls/dir, pwd, ps, …) bằng `job_exec`.
- Builtins: `exit`, `cd <dir>`, `help`, `history` (nếu build với readline), `set <k> <v>`, `get <k>`.
- Hỗ trợ phím điều hướng (left/right/up/down) và autocomplete khi build với `USE_READLINE=1`.
- Xử lý tín hiệu: bỏ qua SIGTSTP/SIGTTOU, in newline khi SIGINT; Ctrl+C ngắt tiến trình con.
- Chế độ nền/foreground: thêm `&` để chạy nền; fg <job_id> để đưa tiến trình lên foreground.
- Quản lý job: thêm vào bảng job, resume/stop, kill bằng lệnh hệ (ps/kill) hoặc `fg`.
- Biến môi trường riêng: lưu trong `env.txt` qua `set`/`get` (độc lập với bash).

## Ngôn ngữ scripting “trang”
- Biến số nguyên, gán: `x = 10`.
- Biểu thức: + - * /, so sánh `< > ==`.
- Câu lệnh in: `in <expr>`.
- Rẽ nhánh: `neu <expr> thi ... het`.
- Vòng lặp: `khi <expr> thi ... het` (while).
- Khối sandbox `trang ... het`:
  - Cô lập stack/biến, cấm lồng nhau.
  - Trả về giá trị cuối (qua thanh ghi TRANG_RET_SLOT), khôi phục trạng thái ngoài.
- Gọi lệnh hệ trong script: dòng đơn từ khóa (vd. `date`, `pwd`) biên dịch thành cmd và thực thi.
- Cú pháp kết thúc lệnh bằng `;` hoặc xuống dòng.
- Tập lệnh mẫu: [code.trang](code.trang), [test.trang](test.trang), [test_trang_return.trang](test_trang_return.trang), [while.trang](while.trang).
### Trang-lang documentation
- Xem đầy đủ tại: https://loegaire.github.io/trang-lang-docs/
## Kiến trúc VM và compiler
- Compiler (`src/compiler.c`): lexer/parser cho từ khóa: `neu/thi/het/khi/trang/in`, toán tử, biến/số.
- Bảng ký hiệu tối đa 1023 biến; slot 1023 dành cho giá trị trả về của `trang`.
- VM (`src/interpreter.c`): stack 1024 phần tử, mảng biến 1024, opcode: CONG, TRU, NHAN, CHIA, BANG, LON, NHO, NHAY, NNS, CAT, LAY, NAP, BO, IN, DUNG, VAO_TRANG, RA_TRANG.
- Chống tràn/rỗng stack, kiểm tra chia 0, kiểm tra cmdIndex hợp lệ.
- Thực thi lệnh hệ từ script qua opcode IN với operand=1.

## Xây dựng & chạy
```bash
# Build (mặc định bật readline)
make

# Build không dùng readline
USE_READLINE=0 make

# Chạy shell
./thinsh

# Chạy script trang
./thinsh
thinsh> trang code.trang
```

## Ghi chú phát triển
- Tệp nguồn: `src/*.c`, header: `include/`.
- Quản lý opcode/tên trong `src/loadopcode.c`.
- Bảng lệnh mẫu cho VM thuần: `src/code.txt`.
- Dọn sạch:
```bash
make clean
```
