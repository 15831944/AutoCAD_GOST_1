#include "aced.h"
#include "rxregsvc.h"
#include "acutads.h"
#include "adscodes.h"
//Объявление функций
static void initApp(void);
static void unloadApp(void);
static void our_app1(void);
static int our_lsp(struct resbuf*);
static int dofun(void);
//макрос ELEMENTS (число элементов массива)
#define ELEMENTS(array) (sizeof(array)/sizeof((array)[0]))
//объявление структуры для хранения
//имен и указателей LISP-функций
struct func_entry
{ACHAR *func_name; int (*func) (struct resbuf *);};
//стандарнтый массив имен и указателей LISP-функций
//приложения (чаще заполняется с помощью acedRegFunc)
static struct func_entry func_table[] = { {L"funstart",our_lsp},};
//функция initApp
static void initApp()
{
	//регистрация новой команды ASTART в группе BOOK
	acedRegCmds->addCommand(L"BOOK",L"ASTART",L"АСТАРТ",ACRX_CMD_TRANSPARENT,our_app1);
}
//функция unloadApp
static void unloadApp()
{
	acedRegCmds->removeGroup(L"BOOK");
}
//пользовательская функци our_app1
static void our_app1()
{
	acedAlert(L"Простая команда\nдля AutoCAD 2016");
}
//LISP-функция, которая вызывается
//с помощью выражения (funstart)
static int our_lsp(resbuf* RB)
{
	acedAlert(L"Простая LISP-функция\nдля AutoCAD 2016");
	acedRetReal(3.14);//в автокад возвращаем 3.14
	return RTNORM;
}
//вычисление индекса LISP-функции в таблице
//(по введенному LISP-выражению)
//и выполнение С-функции по указателю
static int dofun()
{
	struct resbuf *rb = NULL;
	int nfun, rc;

	if ((nfun = acedGetFunCode()) < 0 || nfun >=ELEMENTS(func_table))
	{
		acdbFail(L"Невозможный номер функции в таблице. ");
		return RTERROR;
	}

	rb = acedGetArgs();

	rc = (*func_table[nfun].func)(rb);
	acutRelRb(rb);

	return rc;
}

//ГЛАВНАЯ ЭКСПОРТИРУЕМАЯ ФУНКЦИЯ
extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
{
	switch(msg)
	{
	case AcRx::kInitAppMsg:
		acrxUnlockApplication(pkt);
		acrxRegisterAppMDIAware(pkt);
		initApp();
		break;
	case AcRx::kUnloadAppMsg:
		unloadApp();
		break;
	case AcRx::kLoadDwgMsg:
		acedDefun(L"funstart",0);
		break;
	case AcRx::kInvkSubrMsg:
		dofun();
		break;
	default:
		break;
	}
	return AcRx::kRetOK;
}
