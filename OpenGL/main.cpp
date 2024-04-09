#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

// Xác định kiểu dữ liệu
typedef vec4 point4;
typedef vec4 color4;
using namespace std;

GLuint program;
GLuint model_loc;
GLuint projection_loc;
GLuint view_loc;

mat4 model;
mat4 projection;
mat4 view;

// Hình lập phương
const int soDinhLapPhuong = 36;
// Hình trụ 8 mặt
// 8 mặt chữ nhật, mỗi mặt 2 tam giác, 6 đỉnh: 6 * 8
// 2 đáy, mỗi đáy 8 tam giác = 3 * 8 * 2 đáy
// 6 * 8 + 3 * 8 * 2 = 96
const int soDinhHinhTru = 96;
// 4 * 6 * 8 = 192
const int soDinhHinhCau = 192;
//11 * 3 * 2 + 13 * 6 = 144
const int soDinhCanhQuat = 144;
//
// Danh sách các đỉnh của các tam giác cần vẽ
point4 arrDinhTamGiac[soDinhLapPhuong + soDinhHinhTru + soDinhHinhCau + soDinhCanhQuat];
// Danh sách các màu tương ứng cho các đỉnh trên
color4 arrMauCacDinh[soDinhLapPhuong + soDinhHinhTru + soDinhHinhCau + soDinhCanhQuat];
//Danh sách các vector pháp tuyến ứng với mỗi đỉnh
vec3 arrVecPhapTuyen[soDinhLapPhuong + soDinhHinhTru + soDinhHinhCau + soDinhCanhQuat];

// Danh sách 8 đỉnh của hình lập phương
point4 cacDinhLapPhuong[8];
// Danh sách các màu cho 8 đỉnh hình lập phương
color4 cacMauHinhLapPhuong[8];

// Các đỉnh và 2 tâm
// 2 * 8 + 2 = 18
point4 cacDinhHinhTru[18];
// Danh sách các màu cho 10 mặt hình trụ
color4 cacMauHinhTru[10];

// Hình tròn
point4 cacDinhHinhCau[5][8];
color4 cacMauHinhCau[10];

//Cánh quạt
point4 cacDinhCanhQuat[26];
color4 cacMauCanhQuat[10];

// Vị trí ánh sáng
//point4 viTriAnhSang(0.025, 3, 5, 1.0);
point4 viTriAnhSang(0.025, 10, 5, 1.0);
// Màu gốc ánh sáng
color4 mauAnhSang(1, 1, 1, 1.0);
// Màu khuếch tán
color4 mauAnhSangKhuechTan(1.0, 1.0, 1.0, 1.0);
// Màu gương phản xạ lại
color4 mauAnhSangPhanXaGuong(1.0, 1.0, 1.0, 1.0);

// Màu gốc vật liệu
color4 mauVatLieu;
// Màu khuếch tán vật liệu
color4 mauVatLieuKhuechTan;
// Màu phản xạ gương vật liệu
color4 mauVatLieuPhanXaGuong;
// Độ bóng vật liệu
float doBongVatLieu = 1000.0;

// Màu được trộn từ 2 màu gốc
color4 mauGocCau;
// Màu khuếch tán được trộn từ 2 màu gốc
color4 mauKhuechTanCau;
// Màu phản xạ gương được trộn từ 2 màu gốc
color4 mauPhanXaGuongCau;

void HinhLapPhuong()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	cacDinhLapPhuong[0] = point4(-0.5, -0.5, 0.5, 1.0);
	cacDinhLapPhuong[1] = point4(-0.5, 0.5, 0.5, 1.0);
	cacDinhLapPhuong[2] = point4(0.5, 0.5, 0.5, 1.0);
	cacDinhLapPhuong[3] = point4(0.5, -0.5, 0.5, 1.0);
	cacDinhLapPhuong[4] = point4(-0.5, -0.5, -0.5, 1.0);
	cacDinhLapPhuong[5] = point4(-0.5, 0.5, -0.5, 1.0);
	cacDinhLapPhuong[6] = point4(0.5, 0.5, -0.5, 1.0);
	cacDinhLapPhuong[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	cacMauHinhLapPhuong[0] = color4(0.0, 0.0, 0.0, 1.0); // đen
	cacMauHinhLapPhuong[1] = color4(1.0, 0.0, 0.0, 1.0); // đỏ
	cacMauHinhLapPhuong[2] = color4(1.0, 1.0, 0.0, 1.0); // vàng
	cacMauHinhLapPhuong[3] = color4(0.0, 1.0, 0.0, 1.0); // xanh lá
	cacMauHinhLapPhuong[4] = color4(0.0, 0.0, 1.0, 1.0); // xanh lam
	cacMauHinhLapPhuong[5] = color4(1.0, 0.0, 1.0, 1.0); // tím
	cacMauHinhLapPhuong[6] = color4(1.0, 0.5, 0.0, 1.0); // cam
	cacMauHinhLapPhuong[7] = color4(0.0, 1.0, 1.0, 1.0); // xanh lơ
}

void HinhTru()
{
	float tren = 0.5;
	float duoi = -0.5;
	// Tâm trên
	cacDinhHinhTru[0] = point4(0, tren, 0, 1);

	// For để gán cho các đỉnh cạnh bên
	for (int i = 1; i <= 8; i++)
	{
		float banKinh = 0.5;
		float rad = (i - 1) * 45.0 / 360 * 2 * M_PI;
		// Tính x va z
		float x = banKinh * cosf(rad);
		float z = banKinh * sinf(rad);
		cacDinhHinhTru[i * 2 - 1] = point4(x, tren, z, 1);
		cacDinhHinhTru[i * 2] = point4(x, duoi, z, 1);
	}

	// Tâm dưới
	cacDinhHinhTru[17] = point4(0, duoi, 0, 1);

	cacMauHinhTru[0] = color4(1.0, 0.0, 0.0, 1.0); // đỏ
	cacMauHinhTru[1] = color4(1.0, 1.0, 0.0, 1.0); // vàng
	cacMauHinhTru[2] = color4(0.0, 1.0, 0.0, 1.0); // xanh lá
	cacMauHinhTru[3] = color4(0.0, 0.0, 1.0, 1.0); // xanh lam
	cacMauHinhTru[4] = color4(1.0, 0.0, 1.0, 1.0); // tím
	cacMauHinhTru[5] = color4(1.0, 0.0, 0.0, 1.0); // đỏ
	cacMauHinhTru[6] = color4(1.0, 1.0, 0.0, 1.0); // vàng
	cacMauHinhTru[7] = color4(0.0, 1.0, 0.0, 1.0); // xanh lá
	cacMauHinhTru[8] = color4(0.0, 0.0, 1.0, 1.0); // xanh lam
	cacMauHinhTru[9] = color4(1.0, 0.0, 1.0, 1.0); // tím
}

void HinhCau()
{
	float x, y, z;
	const float r = 0.5;
	const float cv45torad = 45.0 / 360 * 2 * M_PI;
	for (int i = 0; i <= 4; i++)
	{
		y = -r * cosf(i * cv45torad);
		float bk = r * sinf(i * cv45torad);
		//Tính y
		for (int j = 0; j <= 7; j++)
		{
			// Tính x,z
			x = bk * cosf(j * cv45torad);
			z = -bk * sinf(j * cv45torad);
			cacDinhHinhCau[i][j] = point4(x, y, z, 1);
		}
	}

	cacMauHinhCau[0] = color4(1.0, 0.0, 0.0, 1.0); // đỏ
	cacMauHinhCau[1] = color4(1.0, 1.0, 0.0, 1.0); // vàng
	cacMauHinhCau[2] = color4(0.0, 1.0, 0.0, 1.0); // xanh lá
	cacMauHinhCau[3] = color4(0.0, 0.0, 1.0, 1.0); // xanh lam
	cacMauHinhCau[4] = color4(1.0, 0.0, 1.0, 1.0); // tím
	cacMauHinhCau[5] = color4(1.0, 0.0, 0.0, 1.0); // đỏ
	cacMauHinhCau[6] = color4(1.0, 1.0, 0.0, 1.0); // vàng
	cacMauHinhCau[7] = color4(0.0, 1.0, 0.0, 1.0); // xanh lá
	cacMauHinhCau[8] = color4(0.0, 0.0, 1.0, 1.0); // xanh lam
	cacMauHinhCau[9] = color4(1.0, 0.0, 1.0, 1.0); // tím
}

// Start HLP = 0
// Start Hình trụ = 0 + 36
// Start Hình tròn = 36 + 96 = 132
// Start Cánh Quạt = 132 + 192 = 324

int Index = 0;
// Tạo một mặt hình lập phương (đưa đỉnh, pháp tuyến, màu vào mảng thứ tự cho việc chuyển đến bộ nhớ, )
void TaoMotMatHLP(int a, int b, int c, int d)
{
	vec4 u = cacDinhLapPhuong[b] - cacDinhLapPhuong[a];
	vec4 v = cacDinhLapPhuong[c] - cacDinhLapPhuong[b];
	vec3 normal = normalize(cross(u, v));

	arrVecPhapTuyen[Index] = normal; arrMauCacDinh[Index] = cacMauHinhLapPhuong[a]; arrDinhTamGiac[Index] = cacDinhLapPhuong[a]; Index++;
	arrVecPhapTuyen[Index] = normal; arrMauCacDinh[Index] = cacMauHinhLapPhuong[a]; arrDinhTamGiac[Index] = cacDinhLapPhuong[b]; Index++;
	arrVecPhapTuyen[Index] = normal; arrMauCacDinh[Index] = cacMauHinhLapPhuong[a]; arrDinhTamGiac[Index] = cacDinhLapPhuong[c]; Index++;
	arrVecPhapTuyen[Index] = normal; arrMauCacDinh[Index] = cacMauHinhLapPhuong[a]; arrDinhTamGiac[Index] = cacDinhLapPhuong[a]; Index++;
	arrVecPhapTuyen[Index] = normal; arrMauCacDinh[Index] = cacMauHinhLapPhuong[a]; arrDinhTamGiac[Index] = cacDinhLapPhuong[c]; Index++;
	arrVecPhapTuyen[Index] = normal; arrMauCacDinh[Index] = cacMauHinhLapPhuong[a]; arrDinhTamGiac[Index] = cacDinhLapPhuong[d]; Index++;
}

void TaoHinhVuong()  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/
{
	TaoMotMatHLP(1, 0, 3, 2);
	TaoMotMatHLP(2, 3, 7, 6);
	TaoMotMatHLP(3, 0, 4, 7);
	TaoMotMatHLP(6, 5, 1, 2);
	TaoMotMatHLP(4, 5, 6, 7);
	TaoMotMatHLP(5, 4, 0, 1);
}
// Hình trụ
void TaoMatBenHTru(int a, int b, int c, int d, int mau)
{
	vec4 u = cacDinhHinhTru[b] - cacDinhHinhTru[a];
	vec4 v = cacDinhHinhTru[c] - cacDinhHinhTru[b];
	vec3 phapTuyen = normalize(cross(u, v));

	// a -> b -> c -> a -> c -> d
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhTru[mau]; arrDinhTamGiac[Index] = cacDinhHinhTru[a]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhTru[mau]; arrDinhTamGiac[Index] = cacDinhHinhTru[b]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhTru[mau]; arrDinhTamGiac[Index] = cacDinhHinhTru[c]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhTru[mau]; arrDinhTamGiac[Index] = cacDinhHinhTru[a]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhTru[mau]; arrDinhTamGiac[Index] = cacDinhHinhTru[c]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhTru[mau]; arrDinhTamGiac[Index] = cacDinhHinhTru[d]; Index++;
}
void TaoTamGiacDay(int a, int b, int c, int mau)
{
	vec4 u = cacDinhHinhTru[b] - cacDinhHinhTru[a];
	vec4 v = cacDinhHinhTru[c] - cacDinhHinhTru[b];
	vec3 phapTuyen = normalize(cross(u, v));

	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhTru[mau]; arrDinhTamGiac[Index] = cacDinhHinhTru[a]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhTru[mau]; arrDinhTamGiac[Index] = cacDinhHinhTru[b]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhTru[mau]; arrDinhTamGiac[Index] = cacDinhHinhTru[c]; Index++;
}
void TaoHinhTru()
{
	// Mỗi lệnh 6 điểm, 8 lệnh 48 điểm
	TaoMatBenHTru(2, 1, 3, 4, 0);
	TaoMatBenHTru(4, 3, 5, 6, 1);
	TaoMatBenHTru(6, 5, 7, 8, 2);
	TaoMatBenHTru(8, 7, 9, 10, 3);
	TaoMatBenHTru(10, 9, 11, 12, 4);
	TaoMatBenHTru(12, 11, 13, 14, 5);
	TaoMatBenHTru(14, 13, 15, 16, 6);
	TaoMatBenHTru(16, 15, 1, 2, 7);
	// Kết thúc tại điểm 2

	// Mỗi lệnh 3 điểm, 8 lệnh 24 điểm
	// Bắt đầu tại điểm 2
	TaoTamGiacDay(2, 17, 16, 9);
	TaoTamGiacDay(16, 17, 14, 9);
	TaoTamGiacDay(14, 17, 12, 9);
	TaoTamGiacDay(12, 17, 10, 9);
	TaoTamGiacDay(10, 17, 8, 9);
	TaoTamGiacDay(8, 17, 6, 9);
	TaoTamGiacDay(6, 17, 4, 9);
	TaoTamGiacDay(4, 17, 2, 9);

	// Mỗi lệnh 3 điểm, 8 lệnh 24 điểm
	TaoTamGiacDay(1, 0, 3, 8);
	TaoTamGiacDay(3, 0, 5, 8);
	TaoTamGiacDay(5, 0, 7, 8);
	TaoTamGiacDay(7, 0, 9, 8);
	TaoTamGiacDay(9, 0, 11, 8);
	TaoTamGiacDay(11, 0, 13, 8);
	TaoTamGiacDay(13, 0, 15, 8);
	TaoTamGiacDay(15, 0, 1, 8);

	//Tổng 48 + 24 + 24 = 96 = biến soDinhHinhTru
}

// Hình tròn
void TaoMatBenHinhCau(int i, int j, int i1, int j1, int mau)
{
	vec3 phapTuyen;
	vec4 u;
	vec4 v;
	if (i == 0)
	{
		u = cacDinhHinhCau[i1][j1] - cacDinhHinhCau[i][j];
		v = cacDinhHinhCau[i1][j] - cacDinhHinhCau[i][j];
		phapTuyen = normalize(cross(u, v));
	}
	else
	{
		u = cacDinhHinhCau[i][j1] - cacDinhHinhCau[i][j];
		v = cacDinhHinhCau[i1][j1] - cacDinhHinhCau[i][j1];
		phapTuyen = normalize(cross(u, v));
	}
	// a -> b -> c -> a -> c -> d
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhCau[mau]; arrDinhTamGiac[Index] = cacDinhHinhCau[i][j]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhCau[mau]; arrDinhTamGiac[Index] = cacDinhHinhCau[i][j1]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhCau[mau]; arrDinhTamGiac[Index] = cacDinhHinhCau[i1][j1]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhCau[mau]; arrDinhTamGiac[Index] = cacDinhHinhCau[i][j]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhCau[mau]; arrDinhTamGiac[Index] = cacDinhHinhCau[i1][j1]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauHinhCau[mau]; arrDinhTamGiac[Index] = cacDinhHinhCau[i1][j]; Index++;
}
void TaoHinhCau()
{
	for (int j = 0; j <= 6; j++)
	{
		for (int i = 0; i <= 3; i++)
		{
			TaoMatBenHinhCau(i, j, i + 1, j + 1, 4);
		}
	}

	for (int i = 0; i <= 3; i++)
	{
		TaoMatBenHinhCau(i, 7, i + 1, 0, 3);
	}
}

//----------Tạo hình cánh quạt------------
void TaoTamGiacMatCanhQuat(int a, int b, int c, int mau)
{
	vec4 u = cacDinhCanhQuat[b] - cacDinhCanhQuat[a];
	vec4 v = cacDinhCanhQuat[c] - cacDinhCanhQuat[b];
	vec3 phapTuyen = normalize(cross(u, v));

	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauCanhQuat[mau]; arrDinhTamGiac[Index] = cacDinhCanhQuat[a]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauCanhQuat[mau]; arrDinhTamGiac[Index] = cacDinhCanhQuat[b]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauCanhQuat[mau]; arrDinhTamGiac[Index] = cacDinhCanhQuat[c]; Index++;
	//cout << Index << "hello" << endl;
}

void TaoMatBenCanhQuat(int a, int b, int c, int d, int mau)
{
	vec4 u = cacDinhCanhQuat[b] - cacDinhCanhQuat[a];
	vec4 v = cacDinhCanhQuat[c] - cacDinhCanhQuat[b];
	vec3 phapTuyen = normalize(cross(u, v));

	// a -> b -> c -> a -> c -> d
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauCanhQuat[mau]; arrDinhTamGiac[Index] = cacDinhCanhQuat[a]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauCanhQuat[mau]; arrDinhTamGiac[Index] = cacDinhCanhQuat[b]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauCanhQuat[mau]; arrDinhTamGiac[Index] = cacDinhCanhQuat[c]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauCanhQuat[mau]; arrDinhTamGiac[Index] = cacDinhCanhQuat[a]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauCanhQuat[mau]; arrDinhTamGiac[Index] = cacDinhCanhQuat[c]; Index++;
	arrVecPhapTuyen[Index] = phapTuyen; arrMauCacDinh[Index] = cacMauCanhQuat[mau]; arrDinhTamGiac[Index] = cacDinhCanhQuat[d]; Index++;
}
pair<GLfloat, GLfloat> toaDoCanhQuat2D[13];

void CanhQuat() 
{
	//tọa độ Cánh Quạt 2D
	toaDoCanhQuat2D[0] = { 0.15, 0 };
	toaDoCanhQuat2D[1] = { 0.1, 0.05 };
	toaDoCanhQuat2D[2] = { 0, 0.075 };
	toaDoCanhQuat2D[3] = { -0.025, 0.07 };
	toaDoCanhQuat2D[4] = { -0.1, 0.04 };
	toaDoCanhQuat2D[5] = { -0.14, 0.02 };
	toaDoCanhQuat2D[6] = { -0.15, 0 };
	toaDoCanhQuat2D[7] = { -0.08, -0.1 };
	toaDoCanhQuat2D[8] = { -0.08, -0.2 };
	toaDoCanhQuat2D[9] = { -0.1, -0.26 };
	toaDoCanhQuat2D[10] = { 0, -0.24 };
	toaDoCanhQuat2D[11] = { 0.1, -0.15};
	toaDoCanhQuat2D[12] = { 0.125, -0.1};
	//Tạo các đỉnh
	for (int i = 0; i < 13; i++)
	{
		cacDinhCanhQuat[i] = point4(toaDoCanhQuat2D[i].first, 0.5, toaDoCanhQuat2D[i].second, 1.0);
		cacDinhCanhQuat[13 + i] = point4(toaDoCanhQuat2D[i].first, -0.5, toaDoCanhQuat2D[i].second, 1.0);
	}

	//Màu cánh Quạt
	cacMauCanhQuat[0] = color4(1.0, 0.0, 0.0, 1.0); // đỏ
	cacMauCanhQuat[1] = color4(1.0, 1.0, 0.0, 1.0); // vàng
	cacMauCanhQuat[2] = color4(0.0, 1.0, 0.0, 1.0); // xanh lá
	cacMauCanhQuat[3] = color4(0.0, 0.0, 1.0, 1.0); // xanh lam
	cacMauCanhQuat[4] = color4(1.0, 0.0, 1.0, 1.0); // tím
	cacMauCanhQuat[5] = color4(1.0, 0.0, 0.0, 1.0); // đỏ
	cacMauCanhQuat[6] = color4(1.0, 1.0, 0.0, 1.0); // vàng
	cacMauCanhQuat[7] = color4(0.0, 1.0, 0.0, 1.0); // xanh lá
	cacMauCanhQuat[8] = color4(0.0, 0.0, 1.0, 1.0); // xanh lam
	cacMauCanhQuat[9] = color4(1.0, 0.0, 1.0, 1.0); // tím
}


void TaoCanhQuat()
{
	//Tạo 2 mặt cánh quạt
	for (int i = 2; i < 13; i++) {
		TaoTamGiacMatCanhQuat(0, i - 1, i, i % 10);
		TaoTamGiacMatCanhQuat(13, 13 + i - 1, 13 + i, (13 + i) % 10);
	}
	//Tạp mặt bên cánh quạt
	for (int i = 0; i < 13; i++) {
		TaoMatBenCanhQuat(i, (i + 1) % 13, (i + 1) % 13 + 13, i + 13, i % 10);
	}
}

//----------


void generateGeometry(void)
{
	HinhLapPhuong();
	TaoHinhVuong();

	HinhTru();
	TaoHinhTru();
	
	HinhCau();
	TaoHinhCau();
	
	CanhQuat();
	TaoCanhQuat();
}

// Một số phương thức màu sắc, góc
float RGBConvert(float _param)
{
	return _param / 255.0;
}
color4 RGBtoColor(float _R, float _G, float _B)
{
	return color4(RGBConvert(_R), RGBConvert(_G), RGBConvert(_B), 1.0);
}
float DEGtoRAD(float DEG)
{
	return DEG / 360 * 2 * M_PI;
}

void CauMau() //Trộn
{
	//  sử mô hình blinn - Phong
	mauGocCau = mauAnhSang * mauVatLieu;
	mauKhuechTanCau = mauAnhSangKhuechTan * mauVatLieuKhuechTan;
	mauPhanXaGuongCau = mauAnhSangPhanXaGuong * mauVatLieuPhanXaGuong;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, mauGocCau);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, mauKhuechTanCau);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, mauPhanXaGuongCau);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, viTriAnhSang);
	glUniform1f(glGetUniformLocation(program, "Shininess"), doBongVatLieu);
}
GLfloat ti_le_anh_sang = 1;
void TaoVatLieu(color4 mauGoc, color4 mauPhanXa, color4 mauPXGuong, float doBong)
{
	// Nhập 3 màu vào và trộn màu cho vật thể
	mauVatLieu = mauGoc * ti_le_anh_sang;
	mauVatLieuKhuechTan = mauPhanXa;
	mauVatLieuPhanXaGuong = mauPXGuong;
	doBongVatLieu = doBong;
	CauMau();
}

// Hàm vẽ hình lập phương tâm O,  với ma trận _matrix * _kichthuoc
void VeHinhLapPhuong(mat4 _kichthuoc, mat4 _matrix)
{
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * _matrix * _kichthuoc);
	glDrawArrays(GL_TRIANGLES, 0, soDinhLapPhuong);    /*Vẽ các tam giác*/
}
// Hàm vẽ hình trụ tâm O, bán kính đáy 0.5, chiều cao 1 với ma trận _matrix * _kichthuoc
void VeHinhTru(mat4 _kichthuoc, mat4 _matrix)
{
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * _matrix * _kichthuoc);
	glDrawArrays(GL_TRIANGLES, 36, soDinhHinhTru);    /*Vẽ các tam giác*/
}
// Hàm vẽ hình tròn tâm O, bán kính 0.5 với ma trận _matrix * _kichthuoc
void VeHinhCau(mat4 _kichthuoc, mat4 _matrix)
{
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * _matrix * _kichthuoc);
	glDrawArrays(GL_TRIANGLES, 132, soDinhHinhCau);    /*Vẽ các tam giác*/
}
// Hàm vẽ cánh quạt kích cỡ chuẩn với độ dày 1 từ -0.5 đến 0.5 với ma trận _matrix * _kichthuoc
void VeCanhQuat(mat4 _kichthuoc, mat4 _matrix)
{
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * _matrix * _kichthuoc);
	glDrawArrays(GL_TRIANGLES, 324, soDinhCanhQuat);    /*Vẽ các tam giác*/
}

void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arrDinhTamGiac) + sizeof(arrMauCacDinh) + sizeof(arrVecPhapTuyen), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(arrDinhTamGiac), arrDinhTamGiac);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(arrDinhTamGiac), sizeof(arrMauCacDinh), arrMauCacDinh);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(arrDinhTamGiac) + sizeof(arrMauCacDinh), sizeof(arrVecPhapTuyen), arrVecPhapTuyen);


}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo Caug Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(arrDinhTamGiac)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(arrDinhTamGiac) + sizeof(arrMauCacDinh)));

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
}
//vl, kt, pxg
#define BLACK 0
#define RED 1
#define YELLOW 2
#define GREEN 3
#define BLUE 4
#define MAGENTA 5
#define WHITE 6
#define CYAN 7
#define ORANGE 8
#define BROWN 9
#define GRAY 10
#define DARKGREEN 11
#define DARKYELLOW 12
#define MATDAT 13

#define NHUA 0
#define DONG 1
#define BAC 2
#define KHONGCOGI 3

color4 mauGoc[100];
color4 mauPhanXa[10];
color4 mauPXGuong[10];
GLfloat doBong[10];
void setup_color() {
	mauGoc[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	mauGoc[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	mauGoc[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	mauGoc[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	mauGoc[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	mauGoc[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	mauGoc[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	mauGoc[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
	mauGoc[8] = color4(1.0, 0.6, 0.0, 1.0); // orange
	mauGoc[9] = color4(0.3, 0.0, 0.0, 1.0); // brown
	mauGoc[10] = RGBtoColor(220, 220, 220); // gray
	mauGoc[11] = RGBtoColor(0, 100, 0); //dark green
	mauGoc[12] = color4(0.5, 0.5, 0.0, 1.0); //dark green
	mauGoc[13] = RGBtoColor(128, 128, 128); //more gray
	//---------------------
	mauPhanXa[0] = color4(0.01, 0.01, 0.01, 1.0); // nhựa
	mauPhanXa[1] = color4(0.780392, 0.568627, 0.113725, 1.0); // đồng
	mauPhanXa[2] = color4(0.2775, 0.2775, 0.2775, 1.0); // bạc
	mauPhanXa[3] = color4(0, 0, 0, 1.0); // không
	//---------------------
	mauPXGuong[0] = color4(0.5, 0.5, 0.5, 1); //nhựa
	mauPXGuong[1] = color4(0.992157, 0.941176, 0.807843, 1.0); // dồng
	mauPXGuong[2] = color4(0.773911, 0.773911, 0.773911, 1.0); // bạc
	mauPXGuong[3] = color4(0, 0, 0, 1.0); // không
	//-----------------------
	doBong[0] = 32; // nhựa
	doBong[1] = 27.8974; // đồng
	doBong[2] = 89.6; // bạc
	doBong[3] = 0;
}

void TaoVatLieuVer2(GLuint mau, GLuint vatLieu) {
	TaoVatLieu(mauGoc[mau], mauPhanXa[vatLieu], mauPXGuong[vatLieu], doBong[vatLieu]);
	//TaoVatLieu(mauGoc[mau], mauPhanXa[KHONGCOGI], mauPXGuong[KHONGCOGI], doBong[KHONGCOGI]);
}


mat4 view_mat;
mat4 move_mat;
vec4 eye(2.5, 1.6, -3, 1), at(0, 0, 0, 1), up(0, 1, 0, 1);
GLfloat l = -0.01, r = 0.01, b = -0.01, t = 0.01, zN = 0.02, zF = 50;
GLfloat matXY[2]; // doc ngang
vec4 mat_dinhhuong(0, 0, 1, 1);

GLfloat quat_cay_goc_cao = 0, quat_cay_goc_ngang = 0, quat_cay_quay = 0;
GLuint quat_cay_status = 0;
GLfloat quat_cay_status_ngang = 0;

void quat_cay_de() 
{
	TaoVatLieuVer2(GRAY, NHUA);
	VeHinhTru(Scale(0.3, 0.05, 0.3), Translate(0, 0.025, 0));
}
void quat_cay_truc()
{
	TaoVatLieuVer2(WHITE, NHUA);
	VeHinhTru(Scale(0.05, 0.4, 0.05), Translate(0, 0.25, 0));
}
void quat_cay_moto()
{
	TaoVatLieuVer2(GRAY, NHUA);
	VeHinhTru(Scale(0.2, 0.2, 0.2), RotateZ(90));
}
void quat_cay_truc_moto()
{
	TaoVatLieuVer2(WHITE, BAC);
	VeHinhTru(Scale(0.02, 0.02, 0.02), Translate(0, -0.01, 0));
}
void canh_quat(GLfloat alpha)
{
	TaoVatLieuVer2(RED, NHUA);
	VeCanhQuat(Scale(1, 0.01, 1), RotateY(alpha) * RotateX(-10) * Translate(0.15, 0, 0));
}
void quat_cay_num_chan()
{
	TaoVatLieuVer2(BLACK, NHUA);
	VeHinhTru(Scale(0.06, 0.02, 0.06), Translate(0, 0.01, 0));
}
void bo_canh() 
{
	//model = model * Translate(0, 0.12, 0.05);
	quat_cay_truc_moto();
	canh_quat(0);
	canh_quat(120);
	canh_quat(240);
	quat_cay_num_chan();
}
void quat_cay(GLfloat x, GLfloat y, GLfloat z, bool status)
{
	model = model * Translate(x, y, z);
	if (status)
	{
		model = model * RotateY(matXY[1]);
	}
	quat_cay_de();
	model = model * Translate(0.05, 0, 0);
	quat_cay_truc();
	model = model * Translate(0, 0.55, 0) * RotateY(quat_cay_goc_ngang) * RotateZ(quat_cay_goc_cao);
	quat_cay_moto();
	//xoay
	model = model * Translate(-0.12, 0.0, 0) * RotateX(quat_cay_quay) * RotateZ(90);
	bo_canh();
}

//quạt tường
GLfloat quat_tuong_goc_cao = 0, quat_tuong_goc_ngang = 0, quat_tuong_quay = 0;
GLuint quat_tuong_status = 0;
GLfloat quat_tuong_status_ngang = 0;
void quat_tuong_de() 
{
	TaoVatLieuVer2(BROWN, NHUA);
	VeHinhLapPhuong(Scale(0.1, 0.3, 0.1), Translate(0.05, 0, 0));
}
void quat_tuong_truc()
{
	TaoVatLieuVer2(WHITE, NHUA);
	VeHinhTru(Scale(0.05, 0.4, 0.05), Translate(0, 0.25, 0));
}
void quat_tuong_moto()
{
	TaoVatLieuVer2(GRAY, NHUA);
	VeHinhTru(Scale(0.2, 0.2, 0.2), RotateZ(90));
}
void quat_tuong_truc_moto()
{
	TaoVatLieuVer2(WHITE, BAC);
	VeHinhTru(Scale(0.02, 0.02, 0.02), Translate(0, -0.01, 0));
}
void quat_tuong_canh_quat(GLfloat alpha)
{
	TaoVatLieuVer2(ORANGE, NHUA);
	VeCanhQuat(Scale(1, 0.01, 1), RotateY(alpha) * RotateX(-10) * Translate(0.15, 0, 0));
}
void quat_tuong_num_chan()
{
	TaoVatLieuVer2(BLACK, NHUA);
	VeHinhTru(Scale(0.06, 0.02, 0.06), Translate(0, 0.01, 0));
}
void quat_tuong_bo_canh()
{
	//model = model * Translate(0, 0.12, 0.05);
	quat_tuong_truc_moto();
	quat_tuong_canh_quat(0);
	quat_tuong_canh_quat(120);
	quat_tuong_canh_quat(240);
	quat_tuong_num_chan();
}

void quat_tuong(GLfloat x, GLfloat y, GLfloat z, bool status)
{
	model = model * Translate(x, y, z);
	if (status)
	{
		model = model * RotateY(matXY[1]);
	}

	model = model * Translate(-0.1, 0, 0);
	quat_tuong_de();
	model = model * RotateZ(45);
	model = model * Translate(0, -0.1, 0);
	quat_tuong_truc();
	model = model * Translate(0, 0.55, 0) * RotateY(quat_tuong_goc_ngang) * RotateZ(quat_tuong_goc_cao);
	quat_tuong_moto();
	//xoay
	model = model * Translate(-0.12, 0.0, 0) * RotateX(quat_tuong_quay) * RotateZ(90);
	quat_tuong_bo_canh();
}

//quat tran
GLfloat quat_tran_quay = 0;
GLuint quat_tran_status = 0;
void quat_tran_de()
{
	TaoVatLieuVer2(DARKGREEN, BAC);
	VeHinhTru(Scale(0.1, 0.1, 0.1), Translate(0, -0.05, 0));
}
void quat_tran_truc()
{
	TaoVatLieuVer2(GREEN, BAC);
	VeHinhTru(Scale(0.05, 0.3, 0.05), Translate(0, -0.25, 0));
}
void quat_tran_moto()
{
	TaoVatLieuVer2(DARKGREEN, BAC);
	VeHinhCau(Scale(0.2, 0.05, 0.2), mat4());
}
void quat_tran_canh_quat(GLfloat alpha)
{
	TaoVatLieuVer2(GREEN, BAC);
	VeHinhTru(Scale(0.7, 0.01, 0.2), RotateY(alpha) * RotateX(-5) * Translate(0.35, 0, 0));
}
void quat_tran_bo_canh() {
	quat_tran_moto();
	quat_tran_canh_quat(0);
	quat_tran_canh_quat(90);
	quat_tran_canh_quat(180);
	quat_tran_canh_quat(270);
}
void quat_tran(GLfloat x, GLfloat y, GLfloat z, bool status)
{
	model = model * Translate(x, y, z);
	if (status)
	{
		model = model * RotateY(matXY[1]);
	}

	quat_tran_de();
	quat_tran_truc();
	model = model * Translate(0, -0.405, 0) * RotateY(quat_tran_quay);
	quat_tran_bo_canh();
}
GLuint cua_cuon_status = 0;
GLfloat cua_cuon_do_dai = 4;
void cua_cuon()
{
	model = mat4();
	TaoVatLieuVer2(GRAY, BAC);
	VeHinhLapPhuong(Scale(5, cua_cuon_do_dai, 0.05), Translate(2.5, 4 - cua_cuon_do_dai / 2, 0.025));
}

void cay(GLfloat x, GLfloat y, GLfloat z)
{
	model = Translate(x, y, z);
	TaoVatLieuVer2(BROWN, NHUA);
	VeHinhTru(Scale(0.5, 2, 0.5), Translate(0, 1, 0));
	TaoVatLieuVer2(GREEN, NHUA);
	VeHinhCau(Scale(1.5, 1.5, 1.5), Translate(0, 1.75, 0));
	model = model * Translate(-0.25, 0.5, 0) *RotateZ(45);
	TaoVatLieuVer2(BROWN, NHUA);
	VeHinhTru(Scale(0.25, 0.75, 0.25), Translate(0, 0.25, 0));
	TaoVatLieuVer2(GREEN, NHUA);
	VeHinhCau(Scale(0.5, 0.5, 0.5), Translate(0, 0.5, 0));
	model = Translate(x, y, z);
	model = model * Translate(0.25, 0.25, 0) *RotateZ(-45);
	TaoVatLieuVer2(BROWN, NHUA);
	VeHinhTru(Scale(0.25, 0.75, 0.25), Translate(0, 0.25, 0));
	TaoVatLieuVer2(GREEN, NHUA);
	VeHinhCau(Scale(0.5, 0.5, 0.5), Translate(0, 0.5, 0));
}

void ngoai_canh()
{
	glClearColor(0.53 * ti_le_anh_sang, 0.8 * ti_le_anh_sang, 0.92 * ti_le_anh_sang, 1.0); // màu bầu trời

	model = mat4();
	TaoVatLieuVer2(DARKYELLOW, NHUA);
	//nhà
	VeHinhLapPhuong(Scale(0.1, 4.2, 10), Translate(-0.05, 2, 5));
	VeHinhLapPhuong(Scale(0.1, 4.2, 10), Translate(5.05, 2, 5));
	VeHinhLapPhuong(Scale(5, 0.1, 10), Translate(2.5, 4.05, 5));
	VeHinhLapPhuong(Scale(5, 4, 0.1), Translate(2.5, 2, 9.95));
	//mặt đất
	TaoVatLieuVer2(MATDAT, NHUA);
	VeHinhLapPhuong(Scale(100, 0.1, 20), Translate(2.5, -0.05, 10));
	VeHinhLapPhuong(Scale(100, 0.1, 50), Translate(2.5, -0.05, -35));
	VeHinhLapPhuong(Scale(49, 0.1, 4), Translate(-23, -0.05, -2));
	VeHinhLapPhuong(Scale(49, 0.1, 4), Translate(28, -0.05, -2));
	//đường
	TaoVatLieuVer2(BLACK, NHUA);
	VeHinhLapPhuong(Scale(2, 0.1, 4), Translate(2.5, -0.05, -2));
	VeHinhLapPhuong(Scale(100, 0.1, 6), Translate(2.5, -0.05, -7));
	//giá
	TaoVatLieuVer2(ORANGE, BAC);
	VeHinhLapPhuong(Scale(2, 0.5, 5), Translate(4, 0.25, 7.5));
	VeHinhLapPhuong(Scale(1, 0.5, 5), Translate(4.5, 0.75, 7.5));
	//đèn
	TaoVatLieuVer2(WHITE, BAC);
	VeHinhLapPhuong(Scale(0.05, 0.05, 1), Translate(0.05, 3, 5));
	//
	//quầy
	TaoVatLieuVer2(ORANGE, NHUA);
	VeHinhLapPhuong(Scale(2, 1.2, 1), Translate(1, 0.6, 1.25));
	VeHinhLapPhuong(Scale(1, 1.2, 1), Translate(1.5, 0.6, 2.25));
	TaoVatLieuVer2(GRAY, NHUA);
	VeHinhLapPhuong(Scale(0.5, 0.5, 0.5), Translate(0.5, 1.45, 1.25));
	//cây
	cay(-1, 0, 0.5);
	cay(6, 0, 0.5);
	//minh hoa nhà
	model = mat4();
	TaoVatLieuVer2(BLUE, NHUA);
	VeHinhLapPhuong(Scale(5, 6, 10), Translate(-5, 3, 5));
	TaoVatLieuVer2(MAGENTA, NHUA);
	VeHinhLapPhuong(Scale(5, 10, 10), Translate(-12, 5, 5));
	TaoVatLieuVer2(BLUE, NHUA);
	VeHinhLapPhuong(Scale(5, 4, 10), Translate(-19, 2, 5));
	TaoVatLieuVer2(YELLOW, NHUA);
	VeHinhLapPhuong(Scale(5, 5, 10), Translate(-26, 2.5, 5));
	TaoVatLieuVer2(RED, NHUA);
	VeHinhLapPhuong(Scale(5, 7, 10), Translate(-33, 3.5, 5));
	//
	VeHinhLapPhuong(Scale(5, 6, 10), Translate(10, 3, 5));
	TaoVatLieuVer2(DARKGREEN, NHUA);
	VeHinhLapPhuong(Scale(5, 10, 10), Translate(17, 5, 5));
	TaoVatLieuVer2(BLUE, NHUA);
	VeHinhLapPhuong(Scale(5, 4, 10), Translate(24, 2, 5));
	TaoVatLieuVer2(YELLOW, NHUA);
	VeHinhLapPhuong(Scale(5, 5, 10), Translate(30, 2.5, 5));
	TaoVatLieuVer2(BLUE, NHUA);
	VeHinhLapPhuong(Scale(5, 7, 10), Translate(37, 3.5, 5));
}

void loop() {
	//viTriAnhSang = eye;
	//quạt cây
	quat_cay_quay -= quat_cay_status * 1;
	if (quat_cay_quay <= -360) {
		quat_cay_quay += 360;
	}
	if (quat_cay_goc_ngang >= 70) {
		quat_cay_status_ngang = -0.1;
	}
	if (quat_cay_goc_ngang <= -70) {
		quat_cay_status_ngang = 0.1;
	}
	quat_cay_goc_ngang += quat_cay_status_ngang;
	//quạt cây
	quat_tuong_quay -= quat_tuong_status * 1;
	if (quat_tuong_quay <= -360) {
		quat_tuong_quay += 360;
	}
	if (quat_tuong_goc_ngang >= 70) {
		quat_tuong_status_ngang = -0.1;
	}
	if (quat_tuong_goc_ngang <= -70) {
		quat_tuong_status_ngang = 0.1;
	}
	quat_tuong_goc_ngang += quat_tuong_status_ngang;
	quat_tran_quay += quat_tran_status * 0.3;
	if (quat_tran_quay >= 360) {
		quat_tran_quay -= 360;
	}
	//
	if (cua_cuon_status == 1) {
		cua_cuon_do_dai -= 0.01;
	}
	if (cua_cuon_status == 3) {
		cua_cuon_do_dai += 0.01;
	}
	if (cua_cuon_do_dai < 0) {
		cua_cuon_do_dai = 0;
		cua_cuon_status = 2;
	}
	if (cua_cuon_do_dai > 4) {
		cua_cuon_do_dai = 4;
		cua_cuon_status = 0;
	}
	glutPostRedisplay();
};

const int so_luong_quat = 12;
point4 vi_tri_quat[so_luong_quat] = {
	point4(3.5, 0.5, 9, 1.0),
	point4(3.5, 0.5, 7.5, 1.0),
	point4(3.5, 0.5, 6, 1.0),
	point4(4.5, 1, 9, 1.0),
	point4(4.5, 1, 7.5, 1.0),
	point4(4.5, 1, 6, 1.0),
	point4(5, 2, 1, 1.0),
	point4(5, 3, 1, 1.0),
	point4(5, 2, 2, 1.0),
	point4(5, 3, 2, 1.0),
	point4(2, 4, 3, 1.0),
	point4(2, 4, 6, 1.0)
};
int loai_quat[so_luong_quat] = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2 };
bool quat_trang_thai[so_luong_quat] = {}; // 0 là chưa mua, 1 là đã mua

void display(void)
{
	loop();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//const vec3 viewer_pos(0.0, 0.0, 2.0);  /*Trùng với eye của camera*/
	//TaoVatLieu(mauGoc[1], mauPhanXa[1], mauPXGuong[1], doBong[1]);
	// Vẽ mọi thứ ở đây
	ngoai_canh();
	//
	at = eye + normalize(mat_dinhhuong);
	eye.w = at.w = 1;
	//
	for (int i = 0; i < so_luong_quat; i++)
	{
		model = mat4();
		if (quat_trang_thai[i] == false) {
			switch (loai_quat[i]) {
			case 0:
				quat_cay(vi_tri_quat[i].x, vi_tri_quat[i].y, vi_tri_quat[i].z, 0);
				break;
			case 1:
				quat_tuong(vi_tri_quat[i].x, vi_tri_quat[i].y, vi_tri_quat[i].z, 0);
				break;
			case 2:
				quat_tran(vi_tri_quat[i].x, vi_tri_quat[i].y, vi_tri_quat[i].z, 0);
				break;
			default:
				break;
			}
		}
		else
		{
			vec4 tmp = eye + mat_dinhhuong * 0.4;
			tmp.y = 1.2;
			switch (loai_quat[i]) {
			case 0:
				quat_cay(tmp.x, tmp.y-0.2, tmp.z, 1);
				break;
			case 1:
				quat_tuong(tmp.x, tmp.y, tmp.z, 1);
				break;
			case 2:
				quat_tran(tmp.x, tmp.y+0.5, tmp.z, 1);
				break;
			default:
				break;
			}
		}
	}
	cua_cuon();
	eye.w = 1;
	at.w = 1;
	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(l, r, b, t, zN, zF);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case '[':
		cua_cuon_status = 1;
		break;
	case ']':
		cua_cuon_status = 3;
		break;
	case 'w':
		eye = eye + RotateY(matXY[1]) * vec4(0.0, 0.0, 0.2, 1);
		break;
	case 's':
		eye = eye - RotateY(matXY[1]) * vec4(0.0, 0.0, 0.2, 1);
		break;
	case 'd':
		eye = eye + RotateY(matXY[1]) * vec4(0.2, 0.0, 0.0, 1);
		break;
	case 'a':
		eye = eye - RotateY(matXY[1]) * vec4(0.2, 0.0, 0.0, 1);
		break;
	case 'j':
		matXY[1] -= 3;
		if (matXY[1] >= 360) matXY[1] -= 360;
		if (matXY[1] < 0) matXY[1] += 360;
		break;
	case 'l':
		matXY[1] += 3;
		if (matXY[1] >= 360) matXY[1] -= 360;
		if (matXY[1] < 0) matXY[1] += 360;
		break;
	case 'i':
		matXY[0] -= 3;
		if (matXY[0] < -70) {
			matXY[0] = -70;
		}
		break;
	case 'k':
		matXY[0] += 3;
		if (matXY[0] > 70) {
			matXY[0] = 70;
		}
		break;
	case 'x':
		quat_cay_status += 1;
		quat_cay_status %= 4;
		break;
	case 'X':
		if (quat_cay_status_ngang == 0)
			quat_cay_status_ngang = 0.01;
		else quat_cay_status_ngang = 0;
		break;
	case 'z':
		quat_cay_goc_cao += 1;
		if (quat_cay_goc_cao > 20) {
			quat_cay_goc_cao = 20;
		}
		break;
	case 'Z':
		quat_cay_goc_cao -= 1;
		if (quat_cay_goc_cao < -20) {
			quat_cay_goc_cao = -20;
		}
		break;
	case 'v':
		quat_tuong_status += 1;
		quat_tuong_status %= 4;
		break;
	case 'V':
		if (quat_tuong_status_ngang == 0)
			quat_tuong_status_ngang = 0.01;
		else quat_tuong_status_ngang = 0;
		break;
	case 'c':
		quat_tuong_goc_cao += 1;
		if (quat_tuong_goc_cao > 30) {
			quat_tuong_goc_cao = 30;
		}
		break;
	case 'C':
		quat_tuong_goc_cao -= 1;
		if (quat_tuong_goc_cao < -30) {
			quat_tuong_goc_cao = -30;
		}
		break;
	case 'b':
		quat_tran_status += 1;
		quat_tran_status %= 6;
		break;
	case 'q':
		ti_le_anh_sang -= 0.01;
		if (ti_le_anh_sang < 0)
		{
			ti_le_anh_sang = 0;
		}
		break;
	case 'e':
		ti_le_anh_sang += 0.01;
		if (ti_le_anh_sang > 1)
		{
			ti_le_anh_sang = 1;
		}
		break;
	case '1':
		quat_trang_thai[0] = !quat_trang_thai[0];
		break;
	case '2':
		quat_trang_thai[1] = !quat_trang_thai[1];
		break;
	case '3':
		quat_trang_thai[2] = !quat_trang_thai[2];
		break;
	case '4':
		quat_trang_thai[3] = !quat_trang_thai[3];
		break;
	case '5':
		quat_trang_thai[4] = !quat_trang_thai[4];
		break;
	case '6':
		quat_trang_thai[5] = !quat_trang_thai[5];
		break;
	case '7':
		quat_trang_thai[6] = !quat_trang_thai[6];
		break;
	case '8':
		quat_trang_thai[7] = !quat_trang_thai[7];
		break;
	case '9':
		quat_trang_thai[8] = !quat_trang_thai[8];
		break;
	case '0':
		quat_trang_thai[9] = !quat_trang_thai[9];
		break;
	case '-':
		quat_trang_thai[10] = !quat_trang_thai[10];
		break;
	case '=':
		quat_trang_thai[11] = !quat_trang_thai[11];
		break;
	}
	mat_dinhhuong = RotateY(matXY[1]) * RotateX(matXY[0]) * vec4(0, 0, 1, 1);
	mat_dinhhuong.w = 1;
	//cout << "eye " << eye << endl << "at " << at << endl;
	glutPostRedisplay();
	
}


int main(int argc, char **argv)
{
	setup_color();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(720, 720);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Fan Store");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	//glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}
