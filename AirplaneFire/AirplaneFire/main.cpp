#include<iostream>
#include<graphics.h> //easyx
#include<vector>
#include<conio.h>

using namespace std;

constexpr auto swidth = 600;
constexpr auto sheight = 900;
constexpr unsigned int SHP = 3;
constexpr auto hurttime = 1000;//ms


bool PointInRect(int x, int y, RECT& r)
{
	return (r.left <= x && x <= r.right && r.top <= y && y <= r.bottom);
}

//一个开始界面
void Welcome()
{
	LPCTSTR title = _T("飞机大战");
	LPCTSTR tplay = _T("开始游戏");
	LPCTSTR texit = _T("退出游戏");

	RECT tplayr, texitr;
	BeginBatchDraw();
	setbkcolor(WHITE); //设置背景颜色
	cleardevice();
	settextstyle(60, 0, _T("黑体")); //设置文本样式
	settextcolor(BLACK); //设置字体颜色
	outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 10, title);

	settextstyle(40, 0, _T("黑体")); //设置文本样式
	tplayr.left = swidth / 2 - textwidth(tplay) / 2;
	tplayr.right = tplayr.left + textwidth(tplay);
	tplayr.top = sheight / 5 * 2.5;
	tplayr.bottom = tplayr.top + textheight(tplay);

	texitr.left = swidth / 2 - textwidth(texit) / 2;
	texitr.right = texitr.left + textwidth(texit);
	texitr.top = sheight / 5 * 3;
	texitr.bottom = texitr.top + textheight(texit);

	outtextxy(tplayr.left, tplayr.top, tplay);
	outtextxy(texitr.left, texitr.top, texit);

	EndBatchDraw();
	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EM_MOUSE);
		if (mess.lbutton)
		{
			if (PointInRect(mess.x, mess.y, tplayr))
			{
				return;
			}
			else if (PointInRect(mess.x, mess.y, texitr))
			{
				exit(0);
			}
		}
	}
}

bool RectDuangRect(RECT &r1, RECT &r2)
{
	//判断敌机是否相撞
	RECT r;
	r.left = r1.left - (r2.right - r2.left);
	r.right = r1.right;
	r.top = r1.top - (r2.bottom - r2.top);
	r.bottom = r1.bottom;

	return (r.left < r2.left && r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom);

}

//背景、敌机、英雄、子弹
class BK
{
public:
	BK(IMAGE& img)
		:img(img), y(-sheight)
	{

	}
	void Show()
	{
		if (y == 0)
		{
			y = -sheight;
		}

		y += 4; //背景流动
		putimage(0, y, &img);
	}
private:
	IMAGE& img;
	int x, y;
};

class Hero
{
public:
	Hero(IMAGE& img)
		:img(img),HP(SHP)
	{
		//飞机矩形边框初始化
		rect.left = swidth / 2 - img.getwidth() / 2;
		rect.top = sheight - img.getheight();
		rect.right = rect.left + img.getwidth();
		rect.bottom = sheight;
	}
	void Show()
	{
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 4);
		putimage(rect.left, rect.top, &img);
		line(rect.left, rect.top - 4, rect.left + (img.getwidth() / SHP * HP), rect.top - 4);
	}
	void Control()
	{
		ExMessage mess;
		if (peekmessage(&mess, EM_MOUSE)) {
			//更新飞机周围的边框，让鼠标的位置在边框的中心
			rect.left = mess.x - img.getwidth() / 2;
			rect.top = mess.y - img.getheight() / 2;
			rect.right = rect.left + img.getwidth();
			rect.bottom = rect.top + img.getheight();
		}
	}

	bool hurt() 
	{
		HP--;
		return (HP == 0) ? false : true;
	}

	RECT& GetRect() { return rect; }
private:
	IMAGE& img;
	RECT rect;
	unsigned int HP;
};

class Enemy
{
public:
	Enemy(IMAGE& img, int x, IMAGE* &boom)
		:img(img),isdie(false), boomsum(0)
	{
		selfboom[0] = boom[0];
		selfboom[1] = boom[1];
		selfboom[2] = boom[2];
		rect.left = x;
		rect.right = rect.left + img.getwidth();
		rect.top = -img.getheight();
		rect.bottom = 0;
	}
	bool Show()
	{
		if (isdie)
		{
			if (boomsum == 3)
			{
				return false;
			}
			putimage(rect.left, rect.top, selfboom + boomsum);
			boomsum++;
			return true;
		}
		
		if (rect.top >= sheight)
		{
			return false;
		}
		rect.top += 4;
		rect.bottom += 4;
		putimage(rect.left, rect.top, &img);

		return true;
	}
	void Isdie() {
		isdie = true;
	}

	RECT& GetRect() { return rect; }
private:
	IMAGE& img;
	RECT rect;
	IMAGE selfboom[3];

	bool isdie;
	int boomsum;
};

class Bullet
{
public:
	Bullet(IMAGE& img, RECT pr)
		:img(img)
	{
		rect.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
		rect.right = rect.left + img.getwidth();
		rect.top = pr.top - img.getheight();
		rect.bottom = rect.top + img.getheight();
	}

	bool Show()
	{
		if (rect.bottom <= 0)
		{
			return false;
		}
		rect.top -= 3;
		rect.bottom -= 3;
		putimage(rect.left, rect.top, &img);

		return true;
	}
	RECT& GetRect() { return rect; }

protected:
	IMAGE& img;
	RECT rect;
};

class EBullet :public Bullet
{
public:
	EBullet(IMAGE& img, RECT pr)
		: Bullet(img, pr)
	{
		rect.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
		rect.right = rect.left + img.getwidth();
		rect.top = pr.bottom;
		rect.bottom = rect.top + img.getheight();
	}
	bool Show()
	{
		if (rect.top >= sheight)
		{
			return false;
		}
		rect.top += 5;
		rect.bottom += 5;
		putimage(rect.left, rect.top, &img);
		return true;
	}
};

bool AddEnemy(vector<Enemy*> &es, IMAGE& enemyimg, IMAGE* boom)
{
	Enemy* e = new Enemy(enemyimg, abs(rand()) % (swidth - enemyimg.getwidth()),boom);
	for(auto& i : es)
	{
		if (RectDuangRect(i->GetRect(), e->GetRect()))
		{
			delete e;
			return false;
		}
	}
	es.push_back(e);
	return true;

}

void Over(unsigned long kill)
{
	TCHAR* str = new TCHAR[128];
	_stprintf_s(str, 128, _T("分数：%lu"), kill);

	settextcolor(RED);

	outtextxy(swidth / 2 - textwidth(str) / 2, sheight / 5, str);

	//键盘事件的获取（按Enter返回）
	LPCTSTR info = _T("按Enter返回");
	settextstyle(20, 0, _T("黑体"));
	outtextxy(swidth - textwidth(info), sheight - textheight(info), info);

	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EM_KEY);
		// Enter键的编码0x0D
		if (mess.vkcode == 0X0D)
		{
			return;
		}
	}
}

bool Play()
{
	setbkcolor(WHITE);
	cleardevice();
	bool is_play = true;

	IMAGE heroimg, enemyimg, bkimg, bulletimg; //英雄飞机 敌机 背景图 子弹图
	IMAGE eboom[3];
	loadimage(&heroimg, _T("D:\\Bilibili C++ Project\\AirplaneFire\\AirplaneFireimages\\me1.png"));
	loadimage(&enemyimg, _T("D:\\Bilibili C++ Project\\AirplaneFire\\AirplaneFireimages\\enemy1.png"));
	loadimage(&bkimg, _T("D:\\Bilibili C++ Project\\AirplaneFire\\AirplaneFireimages\\bk2.png"), swidth, sheight*2);
	loadimage(&bulletimg, _T("D:\\Bilibili C++ Project\\AirplaneFire\\AirplaneFireimages\\bullet1.png"));

	loadimage(&eboom[0], _T("D:\\Bilibili C++ Project\\AirplaneFire\\AirplaneFireimages\\enemy1_down1.png"));
	loadimage(&eboom[1], _T("D:\\Bilibili C++ Project\\AirplaneFire\\AirplaneFireimages\\enemy1_down2.png"));
	loadimage(&eboom[2], _T("D:\\Bilibili C++ Project\\AirplaneFire\\AirplaneFireimages\\enemy1_down3.png"));


	//背景类
	BK bk = BK(bkimg);
	Hero hp = Hero(heroimg);

	vector<Enemy*> es; //存敌机的数组
	vector<Bullet*> bs; //子弹的数组
	vector<EBullet*> ebs; //敌机子弹数组
	int bsing = 0;
	clock_t hurtlast = clock();

	unsigned long kill = 0;

	for (int i = 0; i < 5; i++)
	{	
		// 随机生成敌机位置，防止敌机不超过屏幕，减去敌机图片的宽度
		AddEnemy(es, enemyimg, eboom);
	}

	while (is_play)
	{
		bsing++;
		//bsing 子弹密度
		if (bsing % 10 == 0)
		{
			// 10帧
			
			bs.push_back(new Bullet(bulletimg, hp.GetRect()));
			
		}

		if (bsing == 60)
		{
			bsing = 0;
			for (auto& i : es)
			{
				ebs.push_back(new EBullet(bulletimg, i->GetRect()));
			}
		}

		BeginBatchDraw();

		bk.Show();
		Sleep(2);
		flushmessage();
		Sleep(2); //刷新完之后，不能马上获取消息，等待一下
		hp.Control();
		// 键盘暂定部分
		if (_kbhit())
		{
			char v = _getch();
			if (v == 0x20)
			{
				Sleep(500);
				while (true)
				{
					if (_kbhit())
					{
						v = _getch();
						if (v == 0x20)
						{
							break;
						}
					}
					Sleep(16);
				}
			}
		}
		
		{
			ExMessage mess;
			
		}
		
		hp.Show();

		auto bsit = bs.begin();
		while (bsit != bs.end())
		{
			if (!(*bsit)->Show())
			{
				bsit = bs.erase(bsit);
			}
			else
			{
				bsit++;
			}
		}

		auto ebsit = ebs.begin();
		while (ebsit != ebs.end())
		{
			
			if (!(*ebsit)->Show())
			{
				ebsit = ebs.erase(ebsit);
			}
			else
			{
				if (RectDuangRect((*ebsit)->GetRect(), hp.GetRect()))
				{
					if (clock() - hurtlast >= hurttime)
					{
						is_play = hp.hurt();
						hurtlast = clock();
					}
				}
				ebsit++;
			}
			
		}

		auto it = es.begin();
		while (it != es.end())
		{
			if (RectDuangRect((*it)->GetRect(), hp.GetRect()))
			{
				if (clock() - hurtlast >= hurttime)
				{
					is_play = hp.hurt();
					hurtlast = clock();
				}
			}
			//敌机碰撞检测
			auto bit = bs.begin();
			while (bit !=bs.end())
			{
				// 打中敌机检测
				if (RectDuangRect((*bit)->GetRect(), (*it)->GetRect()))
				{
					/*delete (*it);
					es.erase(it);
					it = es.begin();*/
					(*it)->Isdie();
					delete (*bit);
					bs.erase(bit);

					kill++; //分数相加

					break;
				}
				bit++;
			}
			if (!(*it)->Show())
			{
				delete (*it);
				es.erase(it);
				it = es.begin();
			}
			it++;
		}
		for (int i = 0; i < 5 - es.size(); i++)
		{
			//es.push_back(new Enemy(enemyimg, abs(rand()) % (swidth - enemyimg.getwidth())));
			AddEnemy(es, enemyimg,eboom);

		}
		

		EndBatchDraw();
	}
	Over(kill);
	return true;
}

int main()
{
	initgraph(swidth, sheight, EX_SHOWCONSOLE);
	bool is_live = true;
	while (is_live) 
	{
		Welcome();
		// Play函数
		is_live = Play();
	}

	return 0;
}