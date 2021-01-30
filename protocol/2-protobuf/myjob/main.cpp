#include <iostream>
#include "addressbook.pb.h"

void makeAddressBook(std::string &data)
{
    AddressBook::AddressBook addressBook;
    AddressBook::Person *person = addressBook.add_persons();
    person->set_name("doudou");
    person->set_id(123456789);
    person->set_email("473917563@qq.com");
    
    AddressBook::PhoneNumber *phoneNumber = person->add_phones();
    phoneNumber->set_number("13085648852");
    phoneNumber->set_type(AddressBook::PhoneType::HOME);

    phoneNumber = person->add_phones();
    phoneNumber->set_number("18645665545");
    phoneNumber->set_type(AddressBook::PhoneType::HOME);

    std::cout << "addressBook.ByteSize()=" << addressBook.ByteSize() << std::endl;;

    data.clear();
    data.resize(addressBook.ByteSize());
    uint8_t *pData = (uint8_t *)data.c_str();
    addressBook.SerializeToArray(pData, data.size());
}

void praseAddressBook(const std::string &str)
{
    AddressBook::AddressBook addressBook;
    addressBook.ParseFromArray(str.c_str(), str.size());
    std::cout << "addressBook.ByteSize()=" << addressBook.ByteSize() << std::endl;;
}


int main()
{
    std::string str;
    makeAddressBook(str);
    praseAddressBook(str);
    return 0;
}