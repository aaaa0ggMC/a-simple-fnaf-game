/*
作者：aaaa0ggMC                 仅在bilibili
更新时间:2020/8/9
文件作用：实现winMusicPlayer.h的函数等等
*/
#include "winMusicPlayer.h"     //可以播放WAV的mmsystem音乐播放器的模型

using namespace std;            //C++标准库的命名空间

MusicPlayer::MusicPlayer(string src,DWORD mode,bool useThread,int waitTime){//请保证没有使用ASYNC当useThread为true!!!
    m_handle = NULL;//设置音乐的句柄为空
    m_src = src;//设置音乐的源文件为src
    m_mode = mode;//设置音乐的模式为mode
    m_play = false;//设置音乐没有在播放中
    m_useThread = useThread;//是否使用线程
    m_waitTime = waitTime;
}

bool MusicPlayer::m_Play(void * arg){
    if(!m_play){//检测是否已经播放了音乐
        m_play = true;//设置正在播放音乐
        bool rt = (bool)PlaySound((LPCSTR)m_src.c_str(),m_handle,(m_useThread == true)?(m_mode|this->Sync):m_mode);//播放！！！
        if(rt == false){//如果播放失败
            cout << "播放\"" << m_src.c_str() << "\"错误" << endl;//输出错误信息
        }
        return rt;//返回信息
    }
    return false;//如果正在播放，返回播放失败
}

bool MusicPlayer::Play(){
    if(m_useThread){
        cout << "new thread!!!" << endl;
        static thread t([this](){this->m_Play(NULL);});
        m_thread = &t;
        m_thread->detach();
    }else{
        return this->m_Play(NULL);
    }
    return true;
}

bool MusicPlayer::Stop(){
    if(m_play){//检测是否已经播放了音乐
        m_play = false;//将音乐设为未播放
        return (bool)PlaySound((LPCSTR)NULL,m_handle,0);//返回停止的结果
    }
    return false;//如果现在已经停止播放了，返回停止失败
}

void MusicPlayer::setMode(DWORD mode){
    if(!m_play)//检测是否已经播放了音乐
        m_mode = mode;//如果没有在播放，设置模式
}

void MusicPlayer::setSource(string src){
    if(!m_play)//检测是否已经播放了音乐
        m_src = src;//如果没有在播放，设置源文件
}

bool MusicPlayer::compare(MusicBase & ano){
    if(ano.m_handle == this->m_handle)//因为句柄唯一，所以比较句柄即可
        return true;
    return false;
}

bool MusicPlayer::isStop(){
    return m_play;
}

/*Musics::Musics(){}
Musics::Musics(Musics & ano){
    Musics(ano.m_musics);//m_musics是一个vector容器
}

Musics::Musics(vector<MusicPlayer> & players){
    for(int i = 0;i < (int)players.size();i++){
        add(players[i]);//依次添加players中的东西
    }
}

void Musics::add(MusicPlayer & player){
    m_musics.push_back(player);//调用vector中的push_back在m_music末端添加Player
}

void Musics::add(string src,DWORD mode){
    m_musics.push_back(MusicPlayer(src,mode));//调用vector中的push_back在m_music末端添加Player
}

void Musics::deleteItem(MusicPlayer & player){
    for(vector<MusicPlayer>::iterator i = m_musics.begin();i < m_musics.end();i++){
        if(i->compare(player)){//比较是否相等
            i->Stop();//停止正在播放的音乐
            m_musics.erase((vector<MusicPlayer>::iterator)i);//删除项
        }
    }
}

void Musics::deleteItem(int index){//和delete(player)差不多，自行理解
    int n = 0;
    for(vector<MusicPlayer>::iterator i = m_musics.begin();i < m_musics.end();i++){
        if(n >= index){
            i->Stop();
            m_musics.erase((vector<MusicPlayer>::iterator)i);
        }
        n++;
    }
}

size_t Musics::getSize(){//返回m_musics的项的数量
    return (size_t)m_musics.size();
}*/


string operator+(std::string a,std::string b){//因为string只有+=,很不方便，所以设计了+
    string c = a;//创建临时变量c
    c += b;//调用+=
    return c;//返回
}

CPlayer::CPlayer()
    : m_hWnd(nullptr)
    , m_dwFrom(0)
    , m_dwError(0)
{
    ZeroMemory(&m_ciOpen,sizeof(MCI_OPEN_PARMS));
}

CPlayer::CPlayer(LPCSTR src,bool useThread)
	: m_hWnd(nullptr)
	, m_dwFrom(0)
	, m_dwError(0)
{
	ZeroMemory(&m_ciOpen,sizeof(MCI_OPEN_PARMS));
	Open(src);
	m_useThread = useThread;
}


CPlayer::~CPlayer()
{
    Stop();
    Close();
}

// 打开设备
BOOL CPlayer::Open(LPCTSTR lpFileName)
{
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)//如果有打开的MCI设备就关闭
		Close();
	m_ciOpen.lpstrDeviceType = NULL;
	m_ciOpen.lpstrElementName = lpFileName;//播放文件路径
	m_ciOpen.lpstrAlias = _T("MusicAlias");
	if (mciSendCommand(0, MCI_OPEN, MCI_DEVTYPE_WAVEFORM_AUDIO, (DWORD)&m_ciOpen))//打开设备时，设备号为0
		return FALSE;
	m_dwFrom = MCI_MAKE_HMS(0, 0, 0);
	return TRUE;
#else
	TCHAR lpszShortPath[MAX_PATH] = { 0 };
	GetShortPathName(lpFileName, lpszShortPath,sizeof (lpszShortPath));// 获得短路经
	mciSendString(TEXT("Stop MusicAlias"), nullptr, 0, m_hWnd);
	std::string lpstrCommand = TEXT("");
	lpstrCommand += TEXT("Open ");
	lpstrCommand += TEXT(lpszShortPath);
	lpstrCommand += TEXT(" Alias MusicAlias");// MusicAlias 是设备别名
	return !(m_dwError = mciSendString(lpstrCommand.c_str(), nullptr, 0, m_hWnd));
#endif
}

// 播放音乐
BOOL CPlayer::Play()
{
#ifdef _USER_MCI_COMMAND
	MCI_PLAY_PARMS mciplayparms;//播放参数结构
	DWORD cdlen = GetLength(MCI_STATUS_LENGTH);//得到文件大小
	DWORD cdto = MCI_MAKE_HMS(MCI_HMS_HOUR(cdlen), MCI_HMS_MINUTE(cdlen), MCI_HMS_SECOND(cdlen));//把文件中读出的大小转换为时间数量
	mciplayparms.dwCallback = NULL;
	mciplayparms.dwFrom = m_dwFrom;//设置起始位置
	mciplayparms.dwTo = cdto;//设置终止位置
	if (m_ciOpen.wDeviceID != 0)//判断是否打开文件
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_PLAY, MCI_TO | MCI_FROM, (DWORD)(LPVOID)&mciplayparms));
#else
	return !(m_dwError = mciSendString(TEXT("Play MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

// 执行MCI_CLOSE操作，关闭MCI设备
BOOL CPlayer::Close()
{
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_CLOSE, 0, 0));
#else
	return !(m_dwError = mciSendString(TEXT("Close MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

// 执行MCI_STOP操作，停止播放音乐
BOOL CPlayer::Stop()
{
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
	{
		if( 0 == mciSendCommand(m_ciOpen.wDeviceID, MCI_STOP, 0, 0) )
			return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, 0));//把播放位置设定为音乐文件的开头（下一次播放操作从文件开头位置开始）
	}
	m_dwFrom = MCI_MAKE_HMS(0, 0, 0);
#else
	return !(m_dwError = mciSendString(TEXT("Stop MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

// 执行MCI_PAUSE操作，暂停播放音乐
BOOL CPlayer::Pause()
{
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
	{
		DWORD dwsf = GetLength(MCI_STATUS_POSITION);
		m_dwFrom = MCI_MAKE_MSF(MCI_MSF_MINUTE(dwsf), MCI_MSF_SECOND(dwsf), MCI_MSF_FRAME(dwsf));
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_PAUSE, 0, 0));
	}
#else
	return !(m_dwError = mciSendString(TEXT("Pause MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

// 得到当前文件状态
DWORD CPlayer::GetLength(DWORD dwItem)
{
#ifdef _USER_MCI_COMMAND
	MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback = (DWORD)m_hWnd;
	mcistatusparms.dwItem = dwItem;
	mcistatusparms.dwReturn = 0;
	m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mcistatusparms);
	return mcistatusparms.dwReturn;
#else
	TCHAR sPosition[256];
	DWORD lLength;
	if(MCI_STATUS_LENGTH == dwItem)
		mciSendString(TEXT("Status MusicAlias length"), sPosition, 255, m_hWnd);
	else if(MCI_STATUS_POSITION == dwItem)
		mciSendString(TEXT("Status MusicAlias position"), sPosition, 255, m_hWnd);
	lLength = _tcstol(sPosition, nullptr, 10);
	return lLength;// 单位是毫秒 mm
#endif
}

void CPlayer::SetWindowHwnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

// 设置音量 0-1000
BOOL CPlayer::SetVolume(DWORD dSize)
{
#ifdef _USER_MCI_COMMAND
#else
	string lpstrCommand = TEXT("");
	lpstrCommand += TEXT("setaudio MusicAlias volume to ");
	lpstrCommand += to_string(dSize);
	return !(m_dwError = mciSendString(lpstrCommand.c_str(), nullptr, 0, m_hWnd));
#endif
}

// 设置进度 单位是毫秒 mm
BOOL CPlayer::SetPos(DWORD dwPos)
{
#ifdef _USER_MCI_COMMAND
#else
    string lpstrCommand = TEXT("");
	lpstrCommand += TEXT("seek MusicAlias volume to ");
	lpstrCommand += to_string(dwPos);
	return !(m_dwError = mciSendString(lpstrCommand.c_str(), nullptr, 0, m_hWnd));
#endif
}

// 静音True为静音，FALSE为取消静音
BOOL CPlayer::SetAudioOnOff(bool AudioOff)
{
#ifdef _USER_MCI_COMMAND
#else
	if (AudioOff)
		return !(m_dwError = mciSendString(TEXT("setaudio MusicAlias off"), nullptr, 0, m_hWnd));
	else
		return !(m_dwError = mciSendString(TEXT("setaudio MusicAlias on"), nullptr, 0, m_hWnd));
#endif
}

// 设置播放速度1-2000
BOOL CPlayer::SetSpeed(DWORD Speed)
{
#ifdef _USER_MCI_COMMAND
#else
	string lpstrCommand;
	lpstrCommand += TEXT("set MusicAlias speed");
	lpstrCommand +=  to_string(Speed);
	return !(m_dwError = mciSendString(lpstrCommand.c_str(), nullptr, 0, m_hWnd));
#endif
}

// 获得错误描述
BOOL CPlayer::GetError(LPTSTR pszText, UINT cchText)
{
	return mciGetErrorString(m_dwError, pszText, cchText);
}
