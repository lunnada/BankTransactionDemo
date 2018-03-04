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

class Account { // �ʻ�
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

class Customer { // �ͻ�
public:
	Customer(string name, string address) : name(name), address(address) { }

	int withdrawCash(string pin, int kind, double amount, ATM &atm);

private:
	string name;
	string address;
	time_t dob;
	DebitCard *card;
};

class DebitCard { // ���п�
public:
	DebitCard(int cardNum, Account &account) : cardNum(cardNum), account(account) { }

private:
	int cardNum;
	int ownedBy;
	Account &account;
};

class CurrentAccount : public Account { // �ֽ��˻�
public:
	CurrentAccount(int type, string pin, int accountNum, double balance, Customer &owner)
		: Account(type,pin, accountNum, balance, owner) { }


	int debit(double amount) {  // ȡǮ
		if (balance >= amount) {
			balance -= amount;
			cout << "ȡ��ɹ����˻����Ϊ��" << balance << "Ԫ��" << endl << endl;
			return 0;
		}
		else
			cout << "���㡣" << endl << endl;

		return -1;
	}

	int credit(double amount) { // ��Ǯ
		balance += amount;
		return 0;
	}
};

class SavingAccount : public Account { // �����ʻ�
public:
	int debit(double amount) {
		return 0;
	}
	int credit(double amount) {
		return 0;
	}
};

class Transaction { // ����ҵ��
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

class WithdrawTransaction : public Transaction { // ȡ��ҵ��
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

class QueryTransaction : public Transaction { // ��ѯҵ��
public:
	void queryProcessing();
private:
	string query;
	int type;
};

class TransferTransaction : public Transaction { // ת��ҵ��
private:
	double amount;
	int accountNum;
};

class PinValidationTransaction : public Transaction { // ����ҵ��
public:
	void pinChange();
private:
	int oldPin;
	int newPin;
};

class Bank { // ����
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

		if (type == 0) { // ȡ��ҵ��
			cout << "ȡ��ҵ��ȡ�����ǣ�" << amount << "Ԫ��" << endl << endl;

			WithdrawTransaction withdrawTransaction(type, amount, *account);
			result = withdrawTransaction.modifies(amount);

			if (result == 0)
				cout << "ҵ�����ɹ���" << endl << endl;
			else
				cout << "ҵ�����ʧ�ܡ�" << endl << endl;
		}

		return result;
	}

private:
	string name;
	Account *account;
};

class ATM { // �Զ���Ա��
public:
	ATM(string location, Bank &bank) : location(location), bank(bank) {}

	bool identifies(string pin) {
		if (bank.validate(pin)) {
			cout << "������ȷ���û��Ϸ���" << endl << endl;
			return true;
		}
		else {
			cout << "��������û����Ϸ���" << endl << endl;
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
	Bank bank("�й�����");
	ATM atm("����·238��", bank); // λ�ã��������ĸ�����

	Customer wang("Xiao Wang", "����·238��");
	CurrentAccount account(1, "123456", 6856, 200.8, wang);  // �˻����ͣ�pin���룬�˺ţ�������
	DebitCard card(5678, account);

	bank.addAccount(&account);
	cout << "�����ɹ�����ǰ����ǣ�" << account.getBalance() << "Ԫ��" << endl << endl;

	wang.withdrawCash("123456", 0, 100, atm); // ���룬ҵ�����ͣ�������̨ATM�ϰ���

	wang.withdrawCash("23456", 0, 100, atm);
	
	wang.withdrawCash("123456", 0, 300, atm);

	return 0;
}
