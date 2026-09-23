//---------------------------------------------------------------------------
/*
 *
 * ELSYS EXPRESSLY DISCLAIMS ALL WARRANITIES OR REPRESENTATIONS WITH RESPECT TO
 * THIS SOFTWARE AND DOCUMENTATION AND ANY SUPPORT OR MAINTENANCE SERVICES THAT
 * ELSYS MAY CHOOSE TO PROVIDE IN RELATION THERETO (INCLUDING, WITHOUT
 * LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE OR THAT THE SOFTWARE: WILL BE
 * ERROR-FREE, WILL OPERATE WITHOUT INTERUPTION, WILL NOT INFRINGE THE RIGHTS OF A
 * THIRD PARTY, OR WILL PRODUCE RESULTS IN CONNECTION WITH ITS USE). FURTHER,
 * ELSYS EXPRESSLY DISCLAIMS ALL EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * (C) Copyright 2005 - 2023 Elsys AG. All rights reserved.
 */
//---------------------------------------------------------------------------
/*--------------------------------------------------------------------------------
  $Id: Attributes.h 45 2025-03-19 12:46:37Z philipp $
  Attribute container.
----------------------------------------------------------------------------------

Attributes are strings that can be assigned to each channel.
        (boardAddress, inputNumber, name) -> string attribute.

Example:
        attr.SetAttribute(0, 1, "Remark", "Any string can be added as an attribute");
        attr.SetAttribute(0, 1, "Baseline", "2.345e-5");

        string x = attr.GetAttribute(0, 1, "Remark");

To clear an attribute, set it to "".
If the attribute is not found, "" is returned.

--------------------------------------------------------------------------------*/
#ifndef Attributes_h
#define Attributes_h Attributes_h
//---------------------------------------------------------------------------------

#include <string>
#include <map>

//---------------------------------------------------------------------------------

class AttributeKey {
   public:
    AttributeKey() {}

    AttributeKey(int boardAddress, int inputNumber, const std::string& name) {
        m_boardAddress = boardAddress;
        m_inputNumber  = inputNumber;
        m_name         = name;
    }

    int boardAddress() const { return m_boardAddress; }
    int inputNumber() const { return m_inputNumber; }
    std::string name() const { return m_name; }

   private:
    int m_boardAddress;
    int m_inputNumber;
    std::string m_name;
};

bool operator<(const AttributeKey& lhs, const AttributeKey& rhs);
bool operator==(const AttributeKey& lhs, const AttributeKey& rhs);

class Attributes {
   public:
    Attributes();
    Attributes(const Attributes&);
    Attributes& operator=(const Attributes&);

    bool operator==(const Attributes& rhs) const;
    bool operator!=(const Attributes& rhs) const { return !(*this == rhs); }

    // Does not allow "" to be stored. This will clear the corresponding entry.
    void SetAttribute(int boardAddress, int inputNumber, const std::string& name, const std::string& value);

    // Stores "" entries. This is used to convey settings of "" over the net.
    void SetAttributeE(int boardAddress, int inputNumber, const std::string& name, const std::string& value);

    std::string GetAttribute(int boardAddress, int inputNumber, const std::string& name) const;

    void Clear();

    class AttributeEnumerator {
       public:
        virtual bool Callback(int index, int boardAddress, int inputNumber, const std::string& name,
                              const std::string& value) = 0;
    };

    void EnumerateAttributes(AttributeEnumerator* callback);

    int Count() const { return static_cast<int>(m_dictionary.size()); }

    typedef std::map<AttributeKey, std::string>::const_iterator const_iterator;
    const_iterator begin() const { return m_dictionary.begin(); }
    const_iterator end() const { return m_dictionary.end(); }

   private:
    std::map<AttributeKey, std::string> m_dictionary;
};

//---------------------------------------------------------------------------------
#endif  // Attributes_h
