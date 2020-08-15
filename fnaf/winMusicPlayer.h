/*
���ߣ�aaaa0ggMC                 ����bilibili
����ʱ��:2020/8/9
�ļ����ã��ṩģ��
*/
#ifndef WINMUSICPLAYER_H_INCLUDED                               //��ֹ��װ��
#define WINMUSICPLAYER_H_INCLUDED                               //��ֹ��װ��
#include <windows.h>                                            //C�⣬���ڴ��ڱ��
#include <mmsystem.h>                                           //C�⣬֧��PlaySoundA(LPCSTR,HMODULE,DWORD)
#include <string>                                               //C++�⣬֧��string
#include <vector>                                               //C++�⣬֧��vector
#include <stdio.h>                                              //C�⣬֧�ָ���IO
#include <iostream>                                             //C++�⣬֧��IO
#include <stdlib.h>
#include <tchar.h>
#include <fcntl.h>
#include <process.h>
#include <thread>

using namespace std;                                            //C++��׼��������ռ�

/**

**/
string operator+(std::string a,std::string b);

class MusicBase{
public:
    virtual bool Play() = 0;
    virtual bool Stop() = 0;
    virtual bool compare(MusicBase & ano) = 0;
    virtual void setMode(DWORD mode) = 0;
    virtual void setSource(string src) = 0;
    virtual bool isStop() = 0;
    virtual bool m_Play(void * arg) = 0;
    bool m_play;
    string m_src;
    DWORD m_mode;
    HMODULE m_handle;
    bool m_useThread;
    thread * m_thread;
};

class MusicPlayer : public MusicBase{
public:
    int m_waitTime;
    const static DWORD UseApplication = SND_APPLICATION;
    const static DWORD Alias = SND_ALIAS;
    const static DWORD AliasID = SND_ALIAS_ID;
    const static DWORD Async = SND_ASYNC;
    const static DWORD FileName = SND_FILENAME;
    const static DWORD Loop = SND_LOOP;
    const static DWORD Memory = SND_MEMORY;
    const static DWORD NoDefaultMusic = SND_NODEFAULT;
    const static DWORD NoStop = SND_NOSTOP;
    const static DWORD NoWait = SND_NOWAIT;
    const static DWORD Purge = SND_PURGE;
    const static DWORD Sync = SND_SYNC;
    virtual bool Play();
    virtual bool Stop();
    virtual bool compare(MusicBase & ano);
    virtual void setMode(DWORD mode);
    virtual void setSource(string src);
    virtual bool isStop();
    virtual bool m_Play(void * arg);
    MusicPlayer(string src = "",DWORD mode = 0,bool useThread = false,int waitTime = -1);
};

class MCIPlayer : public MusicBase{
    bool Open();
    bool Close();
    bool setVolume(unsigned int value);
    int getlength();
    int getNowProcess();
    virtual bool Play();
    virtual bool Stop();
    virtual bool compare(MusicBase & ano);
    virtual void setMode(DWORD mode);
    virtual void setSource(string src);
    virtual bool isStop();
    virtual bool m_Play(void * arg);
    MCIPlayer(string src = "",DWORD mode = 0,bool useThread = false);
    ~MCIPlayer();
};

class CPlayer
{
public:
	CPlayer(LPCSTR src,bool useThread = false);
	~CPlayer();
	CPlayer();
	BOOL Open(LPCSTR lpFileName);//���ļ�
	BOOL Play();//����
	BOOL Close();//�ر�
	BOOL Stop();//ֹͣ
	BOOL Pause();//��ͣ
	BOOL SetVolume(DWORD dSize);
	BOOL SetPos(DWORD dwPos);
	BOOL SetAudioOnOff(bool AudioOff);
	BOOL SetSpeed(DWORD Speed);
	DWORD GetLength(DWORD dwItem);//��ȡ��������
	BOOL GetError(LPTSTR pszText, UINT cchText);
	void SetWindowHwnd(HWND hWnd);//���������ھ��
private:
	MCI_OPEN_PARMS m_ciOpen;//���豸����
	HWND m_hWnd;//�����ھ��
	DWORD m_dwFrom;//������ʼ��
	MCIERROR m_dwError;
	bool m_loop;
	bool m_useThread;
};

/*class Musics{
public:
    Musics();
    Musics(Musics & ano);
    Musics(vector<MusicPlayer> & players);
    void add(MusicPlayer & player);
    void add(string src,DWORD mode);
    void deleteItem(MusicPlayer & player);
    void deleteItem(int index);
    size_t getSize();
    vector<MusicPlayer> m_musics;
};*/

#endif // WINMUSICPLAYER_H_INCLUDED
