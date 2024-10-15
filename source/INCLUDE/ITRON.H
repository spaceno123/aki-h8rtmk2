/*
	Header	itron.h
*/

#ifndef	_ITRON_H_
#define	_ITRON_H_

/* --- データ型 --- */

typedef	char	B;		/* 符号付き８ビット整数 */
typedef	short	H;		/* 符号付き16ビット整数 */
typedef	long	W;		/* 符号付き32ビット整数 */
/*typedef long long	D;	/* 符号付き64ビット整数 */

typedef	unsigned char	UB;	/* 符号無し８ビット整数 */
typedef	unsigned short	UH;	/* 符号無し16ビット整数 */
typedef	unsigned long	UW;	/* 符号無し32ビット整数 */
/*typedef unsigned long long UD;/* 符号無し64ビット整数 */

typedef	UB	VB;		/* データタイプが定まらない８ビットの値 */
typedef	UH	VH;		/* データタイプが定まらない16ビットの値 */
typedef	UW	VW;		/* データタイプが定まらない32ビットの値 */
/*typedef UD	VD;		/* データタイプが定まらない64ビットの値 */

typedef	void 	*VP;		/* データタイプが定まらないものへのポインタ */
typedef	void	(*FP)();	/* プログラムの起動番地（ポインタ） */

typedef	int	INT;		/* プロセッサに自然なサイズの符号付き整数 */
typedef	unsigned int UINT;	/* プロセッサに自然なサイズの符号無し整数 */

typedef	INT	BOOL;		/* 真偽値（ＴＲＵＥまたはＦＡＬＳＥ） */

typedef	INT	FN;		/* 機能コード（符号付き整数） */
typedef	INT	ER;		/* エラーコード（符号付き整数） */
typedef	W	ID;		/* オブジェクトのＩＤ番号（符号付き整数） */
typedef	UH	ATR;		/* オブジェクトの属性（符号無し整数） */
typedef	UH	STAT;		/* オブジェクトの状態（符号無し整数） */
typedef	UH	MODE;		/* サービスコールの動作モード */
typedef	H	PRI;		/* 優先度（符号付き整数） */
typedef	UW	SIZE;		/* メモリ領域のサイズ（符号無し整数） */

typedef	W	TMO;		/* タイムアウト指定（符号付き整数） */
typedef	UW	RELTIM;		/* 相対時間（符号無し整数） */
typedef	UW	SYSTIM;		/* システム時刻（符号無し整数） */

typedef	void	*VP_INT;	/* データタイプが定まらないものへのポインタ */
				/* プロセッサに自然なサイズの符号付き整数 */

typedef	H	ER_BOOL;	/* エラーコードまたは真偽値（符号付き整数） */
typedef	W	ER_ID;		/* エラーコードまたはＩＤ番号 */

typedef	H	ER_UINT;	/* エラーコードまたは符号無し整数 */

typedef	UH	TEXPTN;		/* タスク例外要因ビットパターン（符号無し整数） */
typedef	UH	FLGPTN;		/* イベントフラグのビットパターン（符号無し整数） */
typedef struct t_msg {		/* メールボックスのメッセージヘッダ */
	UW	msgno;		/* ???（未実装）??? */
} T_MSG;
typedef struct t_msg_pri {	/* メールボックスの優先度付きメッセージヘッダ */
	T_MSG	msgque;		/* メッセージヘッダ */
	PRI	msgpri;		/* メッセージ優先度 */
} T_MSG_PRI;
typedef	UH	RDVPTN;		/* ランデブ条件のビットパターン */
typedef	W	RDVNO;		/* ランデブ番号 */
typedef	UW	OVRTIM;		/* プロセッサ時間（符号無し整数、時間単位は実装定義） */
typedef	UW	INHNO;		/* 割り込みハンドラ番号 */
typedef	UW	INTNO;		/* 割り込み番号 */
/*typedef UH	IXXXX;		/* 割り込みマスク */
typedef	UW	EXCNO;		/* ＣＰＵ例外ハンドラ */

#ifndef NULL
#define	NULL	((void *)0)	/* 無効ポインタ */
#endif

#define	TRUE	1		/* 真 */
#define	FALSE	0		/* 偽 */

#define	E_OK	0		/* 正常終了 */

/* --- エラーコード --- */

/*#define	E_SYS	-5		/* システムエラー */

/*#define	E_NOSPT	-9		/* 未サポート機能 */
/*#define	E_RSFN	-10		/* 予約機能コード */
/*#define	E_RSATR	-11		/* 予約属性 */

#define	E_PAR	-17		/* パラメータエラー */
/*#define	E_ID	-18		/* 不正ＩＤ番号 */

#define	E_CTX	-25		/* コンテキストエラー */
/*#define	E_MACV	-26		/* メモリアクセス違反 */
/*#define	E_OACV	-27		/* オブジェクトアクセス違反 */
/*#define	E_ILUSE	-28		/* サービスコール不正使用 */

#define	E_NOMEM	-33		/* メモリ不足 */
#define	E_NOID	-34		/* ＩＤ番号不足 */

#define	E_OBJ	-41		/* オブジェクト状態エラー */
#define	E_NOEXS	-42		/* オブジェクト未生成 */
#define	E_QOVR	-43		/* キューイングオーバフロウ */

#define	E_RLWAI	-49		/* 待ち状態の強制解除 */
#define	E_TMOUT	-50		/* ポーリング失敗またはタイムアウト */
#define	E_DLT	-51		/* 待ちオブジェクトの削除 */
#define	E_CLS	-52		/* 待ちオブジェクトの状態変化 */

#define	E_WBLK	-57		/* ノンブロッキング受け付け */
#define	E_BOVR	-58		/* バッファオーバフロー */

/* --- オブジェクト属性 --- */

#define	TA_NULL	0		/* オブジェクト属性を指定しない */

/* --- タイムアウト指定 --- */

#define	TMO_POL		0	/* ポーリング */
#define	TMO_FEVR	-1	/* 永久待ち */
#define	TMO_NBLK	-2	/* ノンブロッキング */

/* --- エラーコード生成・分解マクロ --- */

#define	ERCD(aaa,bbb)	((bbb)*256+((aaa)&0xff))
#define	MERCD(aaa)	((ER)(B(aaa)))
#define	SERCD(bbb)	((bbb)>>8)

#endif
