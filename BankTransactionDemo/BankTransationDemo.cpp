#include <iostream>
#include <string>
#include <ctime>
using namespace std;

class Bank;
class Customer;
class ATM;
class DebitCard;

class Account;
class CurrentAccount;
class SavingAccount;

class Transaction;
class WithdrawTransaction;
class QueryTransaction;
class TransferTransaction;
class PinValidationTransaction;

class Account { // 帐户
public:
	Account(int type, string pin, int accountNum, double balance, Customer &owner)
		: type(type), pin(pin), accountNum(accountNum), balance(balance), owner(owner) { }

	virtual int debit(double amount) = 0;
	virtual int credit(double amount) = 0;

	string getPin() {
		return pin;
	}

	double getBalance() { return balance; }

protected:
	double balance;

private:
	int type;
	string pin;
	int accountNum;
	Customer &owner;
};

class Customer { // 客户
public:
	Customer(string name, string address) : name(name), address(address) { }

	int withdrawCash(string pin, int kind, double amount, ATM &atm);

private:
	string name;
	string address;
	time_t dob;
	DebitCard *card;
};

class DebitCard { // 银行卡
public:
	DebitCard(int cardNum, Account &account) : cardNum(cardNum), account(account) { }

private:
	int cardNum;
	int ownedBy;
	Account &account;
};

class CurrentAccount : public Account { // 现金账户
public:
	CurrentAccount(int type, string pin, int accountNum, double balance, Customer &owner)
		: Account(type,pin, accountNum, balance, owner) { }


	int debit(double amount) {  // 取钱
		if (balance >= amount) {
			balance -= amount;
			cout << "取款成功，账户余额为：" << balance << "元。" << endl << endl;
			return 0;
		}
		else
			cout << "余额不足。" << endl << endl;

		return -1;
	}

	int credit(double amount) { // 存钱
		balance += amount;
		return 0;
	}
};

class SavingAccount : public Account { // 储蓄帐户
public:
	int debit(double amount) {
		return 0;
	}
	int credit(double amount) {
		return 0;
	}
};

class Transaction { // 银行业务
public:
	Transaction(int type, double amount, Account &account) : type(type), amount(amount), account(account) { }

	virtual int modifies(Account &account, double amount) { return 0; }

protected:
	Account &account;

private:
	int transactionId;
	time_t date;
	int type;
	double amount;
};

class WithdrawTransaction : public Transaction { // 取款业务
public:
	WithdrawTransaction(int type, double amount, Account &account) : Transaction(type, amount, account) { }

	int modifies(double amount) {
		return debit(amount);
	}

private:
	int debit(double amount) {
		return account.debit(amount);
	}

private:
	double amount;
};

class QueryTransaction : public Transaction { // 查询业务
public:
	void queryProcessing();
private:
	string query;
	int type;
};

class TransferTransaction : public Transaction { // 转账业务
private:
	double amount;
	int accountNum;
};

class PinValidationTransaction : public Transaction { // 改密业务
public:
	void pinChange();
private:
	int oldPin;
	int newPin;
};

class Bank { // 银行
public:
	Bank(string name) : name(name) { }

	void addAccount(Account *account) { this->account = account; }

	bool validate(string pin) {
		string accountPin = account->getPin();
		if (pin == accountPin)
			return true;
		return false;
	}

	int processTransaction(int type, double amount) {
		int result = -1;

		if (type == 0) { // 取款业务
			cout << "取款业务。取款金额是：" << amount << "元。" << endl << endl;

			WithdrawTransaction withdrawTransaction(type, amount, *account);
			result = withdrawTransaction.modifies(amount);

			if (result == 0)
				cout << "业务办理成功。" << endl << endl;
			else
				cout << "业务办理失败。" << endl << endl;
		}

		return result;
	}

private:
	string name;
	Account *account;
};

class ATM { // 自动柜员机
public:
	ATM(string location, Bank &bank) : location(location), bank(bank) {}

	bool identifies(string pin) {
		if (bank.validate(pin)) {
			cout << "密码正确，用户合法。" << endl << endl;
			return true;
		}
		else {
			cout << "密码错误，用户不合法。" << endl << endl;
			return false;
		}
	}

	void requestType(int type) {
		this->type = type;
	}

	void requestAmount(double amount) {
		this->amount = amount;
	}

	int processTransaction() {
		return bank.processTransaction(type, amount);
	}

private:
	string location;
	Bank &bank;

	int type;
	double amount;
};

int Customer::withdrawCash(string pin, int type, double amount, ATM &atm) {
	bool isLegal = atm.identifies(pin);

	if (isLegal) {
		atm.requestType(type);
		atm.requestAmount(amount);
		atm.processTransaction();

		return 0;
	}

	return -1;

}

int main(int argc, char *argv[]) {
	Bank bank("中国银行");
	ATM atm("松岭路238号", bank); // 位置，隶属于哪个银行

	Customer wang("Xiao Wang", "松岭路238号");
	CurrentAccount account(1, "123456", 6856, 200.8, wang);  // 账户类型，pin密码，账号，余额，户主
	DebitCard card(5678, account);

	bank.addAccount(&account);
	cout << "开户成功，当前余额是：" << account.getBalance() << "元。" << endl << endl;

	wang.withdrawCash("123456", 0, 100, atm); // 密码，业务类型，金额，在哪台ATM上办理

	wang.withdrawCash("23456", 0, 100, atm);
	
	wang.withdrawCash("123456", 0, 300, atm);

	return 0;
}
