#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <vector>
#include <queue>
#include <string.h>
#include <windows.h>
#include <fstream>
using namespace std;
enum paint_char{black=1,white=2,empty=-1,nempty=-2,side=3,sum=3};
enum vertion{player,computer};
const int chessbored_size=21;//棋盘尺寸 
int chessbored[chessbored_size][chessbored_size];//棋盘 
int cursor_x,cursor_y;//光标坐标 
int player_color=black;//玩家棋子颜色
int computer_color=white; //电脑棋子颜色 
const int max2=2147483647,fen_max=99999999;
const int min2=-2147483647,fen_min=-99999999;
const int wide=2,depth=16;//搜索的深度和广度 
int regret_array[180][chessbored_size][chessbored_size],regret_num;//悔棋记录数组
struct node
{
	int i,j;
	int value;
	int c[2][4];//各方向形成的连接的子数 
	friend bool operator < (node a,node b)
	{
		return a.value<b.value;
	}
	friend bool operator > (node a,node b)
	{
		return a.value>b.value;
	}
} ;
//priority_queue< node,vector<node>,less<node> > empty_player;
//priority_queue< node,vector<node>,less<node> > empty_player;
vector<node> empty_node;
vector<node> regret_empty_node[180];//悔棋 
int szfz(int a[chessbored_size][chessbored_size],int b[chessbored_size][chessbored_size])
{
	int i,j;
	for(i=0;i<chessbored_size;i++)
		for(j=0;j<chessbored_size;j++)
			b[i][j]=a[i][j];
	return 0;
}
int ini_chessbored()//初始化棋盘 
{
	memset(chessbored,-1,sizeof(chessbored));
	int i;
	for(i=0;i<chessbored_size;i++)
	{
		chessbored[i][0]=side;
		chessbored[0][i]=side;
		chessbored[chessbored_size-1][i]=side;
		chessbored[i][chessbored_size-1]=side;
	} 
	return 0;	
} 
string get_style(int i,int j)//得到坐标的字符 
{
	int a=chessbored[i][j];
	if(i==cursor_x&&j==cursor_y)return "  ";
	if(a==black)return "●";
	if(a==white)return "○";
	if(i>1)
	{
		if(i<chessbored_size-2)
		{
			if(j>1)
			{
				if(j<chessbored_size-2)
				{
					return "┼";
				}
				else
				{
					return "┨";
				}
			}
			else 
			{
				return "┠";
			}
		}
		else 
		{
			if(j>1)
			{
				if(j<chessbored_size-2)
				{
					return "┷";
				}
				else
				{
					return "┛";
				}
			}
			else 
			{
				return "┗";
			}
		} 
	}
	else 
	{
		if(j>1)
		{
			if(j<chessbored_size-2)
			{
				return "┯";
			}
			else
			{
				return "┓";
			}
		}
		else 
		{
			return "┏";
		}
	}
} 
int paint()//绘图 
{
	string canvas[chessbored_size];
	int i,j;
	for(i=1;i<chessbored_size-1;i++)
	{
		canvas[i]=canvas[i]+" ";
		for(j=1;j<chessbored_size-1;j++)
			canvas[i]=canvas[i]+get_style(i,j);
		canvas[i]=canvas[i]+" ";
	}
	for(i=0;i<chessbored_size;i++)	
	{
		canvas[0]=canvas[0]+" ";
		canvas[chessbored_size-1]=canvas[chessbored_size-1]+" ";
	}
	system("cls");
	for(i=0;i<chessbored_size;i++)
		cout<<canvas[i]<<endl;
	return 0;	
}
int near_num(int x,int y)//返回此点形成的最高连珠数 
{
	int s,m,k,n=chessbored[x][y];
	s=1;
	for(k=1;k<5&&chessbored[x][y-k]==n;k++)s++;
	for(k=1;k<5&&chessbored[x][y+k]==n;k++)s++;
	m=s;
	s=1;
	for(k=1;k<5&&chessbored[x-k][y]==n;k++)s++;
	for(k=1;k<5&&chessbored[x+k][y]==n;k++)s++;
	if(s>m)m=s;
	s=1;
	for(k=1;k<5&&chessbored[x-k][y-k]==n;k++)s++;
	for(k=1;k<5&&chessbored[x+k][y+k]==n;k++)s++;
	if(s>m)m=s;
	s=1;
	for(k=1;k<5&&chessbored[x-k][y+k]==n;k++)s++;
	for(k=1;k<5&&chessbored[x+k][y-k]==n;k++)s++;
	if(s>m)m=s;
	return m;
}
int empty_value_sub(node e,int a)//a表示视角是玩家还是电脑 ，空格评分函数。 
{
	int temp=chessbored[e.i][e.j];
	chessbored[e.i][e.j]=a;
	if(near_num(e.i,e.j)>4)
	{
		chessbored[e.i][e.j]=temp;
		return fen_max;
	}
	chessbored[e.i][e.j]=temp;
	  int b=1,i,j,k,s=0,n,fa=black+white-a;
	  for(k=1;chessbored[e.i][e.j-k]==a;k++)
  		 b*=4;  	
	  if(chessbored[e.i][e.j-k]<0)
	  	b*=2;
      for(k=1;chessbored[e.i][e.j+k]==a;k++)
	  	b*=4;
	  if(chessbored[e.i][e.j+k]<0)
	  	b*=2;
   	  n=1;
      for(k=1;k<5&&chessbored[e.i][e.j-k]!=fa&&chessbored[e.i][e.j-k]!=side;k++)
 		n++;
	  for(k=1;k<5&&chessbored[e.i][e.j+k]!=fa&&chessbored[e.i][e.j+k]!=side;k++)
 		n++;
	  if(n<5)b=0;
  	  s=s+b;
  	  
  	  b=1;
  	  for(k=1;chessbored[e.i-k][e.j]==a;k++)
	  	b*=4;
	  if(chessbored[e.i-k][e.j]<0)
	  	b*=2;
      for(k=1;chessbored[e.i+k][e.j]==a;k++)
	  	b*=4;
	  if(chessbored[e.i+k][e.j]<0)
	  	b*=2;
  	  n=1;
      for(k=1;k<5&&chessbored[e.i-k][e.j]!=fa&&chessbored[e.i-k][e.j]!=side;k++)
 		n++;
	  for(k=1;k<5&&chessbored[e.i+k][e.j]!=fa&&chessbored[e.i+k][e.j]!=side;k++)
 		n++;
	  if(n<5)b=0;
  	  s=s+b;
  	  
  	  b=1;
  	  for(k=1;chessbored[e.i-k][e.j-k]==a;k++)
	  	b*=4;
	  if(chessbored[e.i-k][e.j-k]<0)
	  	b*=2;
      for(k=1;chessbored[e.i+k][e.j+k]==a;k++)
	  	b*=4;
	  if(chessbored[e.i+k][e.j+k]<0)
	  	b*=2;
  	  n=1;
      for(k=1;k<5&&chessbored[e.i-k][e.j-k]!=fa&&chessbored[e.i-k][e.j-k]!=side;k++)
 		n++;
	  for(k=1;k<5&&chessbored[e.i+k][e.j+k]!=fa&&chessbored[e.i+k][e.j+k]!=side;k++)
 		n++;
	  if(n<5)b=0;
  	  s=s+b;
  	  
  	   b=1;
  	  for(k=1;chessbored[e.i-k][e.j+k]==a;k++)
	  	b*=4;
	  if(chessbored[e.i-k][e.j+k]<0)
	  	b*=2;
      for(k=1;chessbored[e.i+k][e.j-k]==a;k++)
	  	b*=4;
	  if(chessbored[e.i+k][e.j-k]<0)
	  	b*=2;
  	  n=1;
      for(k=1;k<5&&chessbored[e.i-k][e.j+k]!=fa&&chessbored[e.i-k][e.j+k]!=side;k++)
 		n++;
	  for(k=1;k<5&&chessbored[e.i+k][e.j-k]!=fa&&chessbored[e.i+k][e.j-k]!=side;k++)
 		n++;
	  if(n<5)b=0;
  	  s=s+b;
  	  
  	  e.value=s;
  	  return s;
}
int empty_value(node &e,int a)
{
	int my,you;
	my=empty_value_sub(e,a);
	you=empty_value_sub(e,player_color+computer_color-a);
	e.value=my+you/2;
	return my+you/2;
}
int empty_value2(node &e,int a)//评分函数 
{
	int my,you;
	my=empty_value_sub(e,a);
	you=empty_value_sub(e,player_color+computer_color-a);
	e.value=my-you;
	return my-you;
}
int pingfen(int pc)//总评函数 
{
	vector<node>::iterator the_iterator;
	node e;
	int max3=fen_min;
	the_iterator = empty_node.begin();
 	while( the_iterator != empty_node.end() ) 
	{
		e=*the_iterator;
		empty_value2(e,pc);
		if(e.value>max3)
			max3=e.value;
 	   	the_iterator++;
   }
   return max3;
}
int display()
{
	vector<node>::iterator the;
	the=empty_node.begin();
	while(the!=empty_node.end())
	{
		cout<<(*the).i<<" "<<(*the).j<<" "<<(*the).value<<endl;
		the++;
	}
	return 0;
}
int before_move(int x,int y)
{
	if(chessbored[x][y]==nempty)
	{
		vector<node>::iterator the_iterator;
		node e;
		the_iterator = empty_node.begin();
	    while( the_iterator != empty_node.end() ) 
		{
			e=*the_iterator;
			if(e.i==x&&e.j==y)
			{
				empty_node.erase(the_iterator);
				break;
			}
 	    	the_iterator++;
 	    }
	}
	return 0;
}
int updata_empty_node(int x,int y)//更新棋子周围的空点 
{
	int i,j;
	for(i=-2;i<3;i++)
		for(j=-2;j<3;j++)
			if(chessbored[x+i][y+j]==empty&&x+i>0&&x+i<20&&y+j>0&&y+j<20)
			{
				node e;
				e.i=x+i;
				e.j=y+j;
				empty_node.push_back(e);
				//cout<<e.i<<" "<<e.j<<endl; 
				chessbored[x+i][y+j]=nempty;
			}
	return 0;
}
int updata_empty_node(int x,int y,int save[25][3],int &savenum)//更新棋子周围的空点 
{
	int i,j,cnt=0;
	for(i=-2;i<3;i++)
		for(j=-2;j<3;j++)
			if(chessbored[x+i][y+j]==empty&&x+i>0&&x+i<20&&y+j>0&&y+j<20)
			{
				node e;
				e.i=x+i;
				e.j=y+j;
				empty_node.push_back(e);
				save[cnt][0]=x+i;
				save[cnt][1]=y+j;
				save[cnt++][2]=chessbored[x+i][y+j];
				//cout<<e.i<<" "<<e.j<<endl; 
				chessbored[x+i][y+j]=nempty;
			}
	savenum=cnt;
	return 0;
}
int youfen(int pc,int depth2,int jz);
int myfen(int pc,int depth2,int jz)//我的分数 
{
	if(depth2>=depth)
	{
		int a=pingfen(pc);
		return a;
	}
	int fa=player_color+computer_color-pc,c;
	int n=empty_node.size();
    int i,j;
    vector<node> en;
	en=empty_node;
    node e,e2;
    priority_queue< node ,vector<node> ,less<node> >  pen;
    for(i=0;i<n;i++)
    {
    	e=en.back();
    	en.pop_back();
    	empty_value(e,pc);
    	pen.push(e);
    }
    int max_value=min2;
    int wide2=wide;
    if(wide2>n)wide2=n;
    for(i=0;i<wide2;i++)
	{
		e=pen.top();
		vector<node> empty_node2;
		empty_node2=empty_node;
		before_move(e.i,e.j);
		c=chessbored[e.i][e.j];
		chessbored[e.i][e.j]=pc;
		if(near_num(e.i,e.j)>4)
		{	
			empty_node=empty_node2;
			chessbored[e.i][e.j]=c;			
			return fen_max;
		}
		int save[25][3],savenum;
		updata_empty_node(e.i,e.j,save,savenum);	
		int fenshu;
		fenshu=youfen(pc,depth2+1,max_value);
		if(fenshu>max_value)
		{
			if(fenshu>=jz)
			{
				empty_node=empty_node2;
				chessbored[e.i][e.j]=c;	
				for(j=0;j<savenum;j++)
					chessbored[save[j][0]][save[j][1]]=save[j][2];
				return fenshu;
			}				
			max_value=fenshu;
		} 
		empty_node=empty_node2;
		chessbored[e.i][e.j]=c;	
		for(j=0;j<savenum;j++)
			chessbored[save[j][0]][save[j][1]]=save[j][2];
		pen.pop();
	} 
	return max_value;
} 
int youfen(int pc,int depth2,int jz)//你的分数 ,jz剪枝 
{
	int fa=player_color+computer_color-pc;
	int n=empty_node.size();
    int i,j;
    vector<node> en;
	en=empty_node;
    node e,e2;
    priority_queue< node ,vector<node> ,less<node> >  pen;
    for(i=0;i<n;i++)
    {
    	e=en.back();
    	en.pop_back();
    	empty_value(e,fa);
    	pen.push(e);
    }
    int min_value=max2,c;
    int wide2=wide;
    if(wide2>n)wide2=n;
    for(i=0;i<wide2;i++)
	{
		e=pen.top();
		vector<node> empty_node2;
		empty_node2=empty_node;
		before_move(e.i,e.j);
		c=chessbored[e.i][e.j];
		chessbored[e.i][e.j]=fa;
		if(near_num(e.i,e.j)>4)
		{	
			empty_node=empty_node2;
			chessbored[e.i][e.j]=c;		
			return fen_min;
		}
		int save[25][3],savenum;
		updata_empty_node(e.i,e.j,save,savenum);
		int fenshu;
		fenshu=myfen(pc,depth2+1,min_value);
		if(fenshu<min_value)
		{
			if(fenshu<=jz)
			if(fenshu>=jz)
			{
				empty_node=empty_node2;
				chessbored[e.i][e.j]=c;	
				for(j=0;j<savenum;j++)
					chessbored[save[j][0]][save[j][1]]=save[j][2];
				return fenshu;
			}		
			min_value=fenshu;
		} 
		empty_node=empty_node2;
		chessbored[e.i][e.j]=c;	
		for(j=0;j<savenum;j++)
			chessbored[save[j][0]][save[j][1]]=save[j][2];
		pen.pop();
	} 
	return min_value;
}
node get_bestnode(int pc)//得到最优点 ,pc表示以哪一方为自己 
{
	int n=empty_node.size();
    int i,c;
    vector<node> en;
	en=empty_node;
    node e,e2;
    priority_queue< node ,vector<node> ,less<node> >  pen;
    for(i=0;i<n;i++)
    {
    	e=en.back();
    	en.pop_back();
    	empty_value(e,pc);
    	pen.push(e);
    }
    int max_value=min2;
 //   display();
    int wide2=wide;
    if(wide2>n)wide2=n;
    for(i=0;i<wide2;i++)
	{
		e=pen.top();
		vector<node> empty_node2;
		empty_node2=empty_node;
		before_move(e.i,e.j);
		c=chessbored[e.i][e.j];
		chessbored[e.i][e.j]=pc;
		if(near_num(e.i,e.j)>4)
		{		
			empty_node=empty_node2;
			chessbored[e.i][e.j]=c;		
			return e;
		}
		int save[25][3],savenum;
		updata_empty_node(e.i,e.j,save,savenum);	
		int fenshu;
		fenshu=youfen(pc,1,max_value);
		if(fenshu>max_value)
		{
			e2=e;
			max_value=fenshu;
		} 
		empty_node=empty_node2;
		chessbored[e.i][e.j]=c;	
		for(int j=0;j<savenum;j++)
			chessbored[save[j][0]][save[j][1]]=save[j][2];
		pen.pop();
	} 
	return e2;
}
int computer_move(int pc)
{
	node e;
	e=get_bestnode(pc);
	before_move(e.i,e.j);
	chessbored[e.i][e.j]=pc;
	paint();
	if(near_num(e.i,e.j)>4)
		return 1;
	updata_empty_node(e.i,e.j);
	return 0;
}
int regret_save() //储存悔棋信息 
{
	regret_num++;
	szfz(chessbored,regret_array[regret_num]);
	regret_empty_node[regret_num]=empty_node;	
	return 0;
}
int regret()//悔棋 
{
	regret_num--;
	if(regret_num<0)return 0;
	szfz(regret_array[regret_num],chessbored); 
	empty_node=regret_empty_node[regret_num];
	return 0;
}
int player_move()
{
	int key,i;
	while(1)
	{
		key=getch();
		if(key==224)//如果按下的是方向键
		{
			key=getch();
			if(key==72)// 上 
			{
				cursor_x--;
				if(cursor_x<1)
					cursor_x=chessbored_size-2;
			}
			else if(key==80)//下 
			{
				cursor_x++;
				if(cursor_x>chessbored_size-2)
					cursor_x=1;
			}
			else if(key==75)//左 
			{
				cursor_y--;
				if(cursor_y<1)
					cursor_y=chessbored_size-2;
			}
			else if(key==77)//右 
			{
				cursor_y++;
				if(cursor_y>chessbored_size-2)
					cursor_y=1;
			}
			paint();
		} 
		else if(key==32&&chessbored[cursor_x][cursor_y]<0)//如果按下的是空格 
		{
			before_move(cursor_x,cursor_y);
			chessbored[cursor_x][cursor_y]=player_color;
			paint();
			if(near_num(cursor_x,cursor_y)>4)
				return 1;
			updata_empty_node(cursor_x,cursor_y);
			return 0;
		}
		else if(key==27)
		{
			regret();
			paint();
		}
	}
} 
int main()
{
	system("title 五子棋 ——最爱吃兽奶制作");//设置标题
  //  system("mode con cols=45 lines=24");//设置窗口大小
    system("color b0");//设置颜色
    while(1) 
    {
    	ini_chessbored();
    	regret_num=-1;
//		cursor_x=chessbored_size/2;
//		cursor_y=chessbored_size/2;
		cursor_x=2;
		cursor_y=2;
		empty_node.clear();
		cout<<"1先手，2后手"<<endl;
		int cnt;
		cin>>cnt;
		if(cnt==2)
		{
			int f=chessbored_size/2;
			before_move(f,f);
			chessbored[f][f]=computer_color;
			updata_empty_node(f,f);
		}
		paint();
		while(1)
		{
			
			int a;
//			a=computer_move(player_color);
			regret_save();
			a=player_move();			
			if(a)
			{
				cout<<"玩家胜利"<<endl;
				int temp=1;
				if(getch()==27)
				{
					regret();
					paint();
					player_move();
					temp=0;
				}
				if(temp)				
					break; 
			}
//			system("pause");
			a=computer_move(computer_color);
			if(a)
			{
				cout<<"电脑胜利"<<endl;
				int temp=1;
				if(getch()==27)
				{
					regret();
					paint();
					temp=0;
				}
				if(temp)				
					break;  
			}
//			system("pause");
		}	
    }	
	return 0;
}