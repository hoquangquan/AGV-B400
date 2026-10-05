# Hướng dẫn Biên dịch Báo cáo LaTeX AGV B400

Thư mục này chứa file báo cáo kỹ thuật toàn diện dạng LaTeX cho dự án **AGV B400 Project**.

## 📁 Cấu trúc thư mục
- `AGV_B400_Technical_Report.tex`: File nguồn LaTeX chính (chứa toàn bộ nội dung phân tích kiến trúc phần mềm, sơ đồ vận hành, danh mục phần cứng linh kiện).

## 🛠 Cách Biên dịch ra PDF (Compile PDF)

### Cách 1: Sử dụng Overleaf (Khuyên dùng - Nhanh nhất)
1. Truy cập [Overleaf.com](https://www.overleaf.com/) và đăng nhập.
2. Tạo dự án mới (*New Project* -> *Blank Project*).
3. Copy toàn bộ nội dung file `AGV_B400_Technical_Report.tex` dán vào Overleaf.
4. Chọn biên dịch (*Recompile*) với Trình biên dịch **pdfLaTeX** hoặc **XeLaTeX**.

### Cách 2: Biên dịch trên máy cục bộ (Offline)
Nếu máy tính đã cài đặt **MiKTeX**, **TeX Live** hoặc **MacTeX**:
- Mở bằng **TeXstudio** / **VS Code (LaTeX Workshop)** và nhấn `Build/Compile`.
- Hoặc chạy lệnh trong Terminal:
  ```bash
  pdflatex AGV_B400_Technical_Report.tex
  pdflatex AGV_B400_Technical_Report.tex
  ```

---
*Báo cáo được biên soạn tự động từ phân tích mã nguồn thực tế của dự án AGV B400.*
