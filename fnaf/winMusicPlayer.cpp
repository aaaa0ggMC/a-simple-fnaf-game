/*
���ߣ�aaaa0ggMC                 ����bilibili
����ʱ��:2020/8/9
�ļ����ã�ʵ��winMusicPlayer.h�ĺ����ȵ�
*/
#include "winMusicPlayer.h"     //���Բ���WAV��mmsystem���ֲ�������ģ��

using namespace std;            //C++��׼��������ռ�

MusicPlayer::MusicPlayer(string src,DWORD mode,bool useThread,int waitTime){//�뱣֤û��ʹ��ASYNC��useThreadΪtrue!!!
    m_handle = NULL;//�������ֵľ��Ϊ��
    m_src = src;//�������ֵ�Դ�ļ�Ϊsrc
    m_mode = mode;//�������ֵ�ģʽΪmode
    m_play = false;//��������û���ڲ�����
    m_useThread = useThread;//�Ƿ�ʹ���߳�
    m_waitTime = waitTime;
}

bool MusicPlayer::m_Play(void * arg){
    if(!m_play){//����Ƿ��Ѿ�����������
        m_play = true;//�������ڲ�������
        bool rt = (bool)PlaySound((LPCSTR)m_src.c_str(),m_handle,(m_useThread == true)?(m_mode|this->Sync):m_mode);//���ţ�����
        if(rt == false){//�������ʧ��
            cout << "����\"" << m_src.c_str() << "\"����" << endl;//���������Ϣ
        }
        return rt;//������Ϣ
    }
    return false;//������ڲ��ţ����ز���ʧ��
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
    if(m_play){//����Ƿ��Ѿ�����������
        m_play = false;//��������Ϊδ����
        return (bool)PlaySound((LPCSTR)NULL,m_handle,0);//����ֹͣ�Ľ��
    }
    return false;//��������Ѿ�ֹͣ�����ˣ�����ֹͣʧ��
}

void MusicPlayer::setMode(DWORD mode){
    if(!m_play)//����Ƿ��Ѿ�����������
        m_mode = mode;//���û���ڲ��ţ�����ģʽ
}

void MusicPlayer::setSource(string src){
    if(!m_play)//����Ƿ��Ѿ�����������
        m_src = src;//���û���ڲ��ţ�����Դ�ļ�
}

bool MusicPlayer::compare(MusicBase & ano){
    if(ano.m_handle == this->m_handle)//��Ϊ���Ψһ�����ԱȽϾ������
        return true;
    return false;
}

bool MusicPlayer::isStop(){
    return m_play;
}

/*Musics::Musics(){}
Musics::Musics(Musics & ano){
    Musics(ano.m_musics);//m_musics��һ��vector����
}

Musics::Musics(vector<MusicPlayer> & players){
    for(int i = 0;i < (int)players.size();i++){
        add(players[i]);//�������players�еĶ���
    }
}

void Musics::add(MusicPlayer & player){
    m_musics.push_back(player);//����vector�е�push_back��m_musicĩ�����Player
}

void Musics::add(string src,DWORD mode){
    m_musics.push_back(MusicPlayer(src,mode));//����vector�е�push_back��m_musicĩ�����Player
}

void Musics::deleteItem(MusicPlayer & player){
    for(vector<MusicPlayer>::iterator i = m_musics.begin();i < m_musics.end();i++){
        if(i->compare(player)){//�Ƚ��Ƿ����
            i->Stop();//ֹͣ���ڲ��ŵ�����
            m_musics.erase((vector<MusicPlayer>::iterator)i);//ɾ����
        }
    }
}

void Musics::deleteItem(int index){//��delete(player)��࣬�������
    int n = 0;
    for(vector<MusicPlayer>::iterator i = m_musics.begin();i < m_musics.end();i++){
        if(n >= index){
            i->Stop();
            m_musics.erase((vector<MusicPlayer>::iterator)i);
        }
        n++;
    }
}

size_t Musics::getSize(){//����m_musics���������
    return (size_t)m_musics.size();
}*/


string operator+(std::string a,std::string b){//��Ϊstringֻ��+=,�ܲ����㣬���������+
    string c = a;//������ʱ����c
    c += b;//����+=
    return c;//����
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

// ���豸
BOOL CPlayer::Open(LPCTSTR lpFileName)
{
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)//����д򿪵�MCI�豸�͹ر�
		Close();
	m_ciOpen.lpstrDeviceType = NULL;
	m_ciOpen.lpstrElementName = lpFileName;//�����ļ�·��
	m_ciOpen.lpstrAlias = _T("MusicAlias");
	if (mciSendCommand(0, MCI_OPEN, MCI_DEVTYPE_WAVEFORM_AUDIO, (DWORD)&m_ciOpen))//���豸ʱ���豸��Ϊ0
		return FALSE;
	m_dwFrom = MCI_MAKE_HMS(0, 0, 0);
	return TRUE;
#else
	TCHAR lpszShortPath[MAX_PATH] = { 0 };
	GetShortPathName(lpFileName, lpszShortPath,sizeof (lpszShortPath));// ��ö�·��
	mciSendString(TEXT("Stop MusicAlias"), nullptr, 0, m_hWnd);
	std::string lpstrCommand = TEXT("");
	lpstrCommand += TEXT("Open ");
	lpstrCommand += TEXT(lpszShortPath);
	lpstrCommand += TEXT(" Alias MusicAlias");// MusicAlias ���豸����
	return !(m_dwError = mciSendString(lpstrCommand.c_str(), nullptr, 0, m_hWnd));
#endif
}

// ��������
BOOL CPlayer::Play()
{
#ifdef _USER_MCI_COMMAND
	MCI_PLAY_PARMS mciplayparms;//���Ų����ṹ
	DWORD cdlen = GetLength(MCI_STATUS_LENGTH);//�õ��ļ���С
	DWORD cdto = MCI_MAKE_HMS(MCI_HMS_HOUR(cdlen), MCI_HMS_MINUTE(cdlen), MCI_HMS_SECOND(cdlen));//���ļ��ж����Ĵ�Сת��Ϊʱ������
	mciplayparms.dwCallback = NULL;
	mciplayparms.dwFrom = m_dwFrom;//������ʼλ��
	mciplayparms.dwTo = cdto;//������ֹλ��
	if (m_ciOpen.wDeviceID != 0)//�ж��Ƿ���ļ�
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_PLAY, MCI_TO | MCI_FROM, (DWORD)(LPVOID)&mciplayparms));
#else
	return !(m_dwError = mciSendString(TEXT("Play MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

// ִ��MCI_CLOSE�������ر�MCI�豸
BOOL CPlayer::Close()
{
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_CLOSE, 0, 0));
#else
	return !(m_dwError = mciSendString(TEXT("Close MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

// ִ��MCI_STOP������ֹͣ��������
BOOL CPlayer::Stop()
{
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
	{
		if( 0 == mciSendCommand(m_ciOpen.wDeviceID, MCI_STOP, 0, 0) )
			return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, 0));//�Ѳ���λ���趨Ϊ�����ļ��Ŀ�ͷ����һ�β��Ų������ļ���ͷλ�ÿ�ʼ��
	}
	m_dwFrom = MCI_MAKE_HMS(0, 0, 0);
#else
	return !(m_dwError = mciSendString(TEXT("Stop MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

// ִ��MCI_PAUSE��������ͣ��������
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

// �õ���ǰ�ļ�״̬
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
	return lLength;// ��λ�Ǻ��� mm
#endif
}

void CPlayer::SetWindowHwnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

// �������� 0-1000
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

// ���ý��� ��λ�Ǻ��� mm
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

// ����TrueΪ������FALSEΪȡ������
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

// ���ò����ٶ�1-2000
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

// ��ô�������
BOOL CPlayer::GetError(LPTSTR pszText, UINT cchText)
{
	return mciGetErrorString(m_dwError, pszText, cchText);
}
