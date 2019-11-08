#include "ucioption.h"

#include <algorithm>
#include <cassert>

namespace engine
{

UciOption::UciOption(bool check, Callback<bool> callback)
    : _type(kCHECK), _check(check), _check_callback(callback)
{
    _check_callback(_check);
}

UciOption::UciOption(int initial, int min, int max, Callback<int> callback)
    : _type(kSPIN), _spin(initial), _spin_min(min), _spin_max(max), _spin_callback(callback)
{
    _spin_callback(_spin);
}

UciOption::UciOption(CallbackVoid callback)
    : _type(kBUTTON), _button_callback(callback)
{
}

UciOption::UciOption(std::string initial, std::vector<std::string> options, Callback<std::string> callback)
    : _type(kCOMBO), _combo_options(options), _string(initial), _string_callback(callback)
{
    _string_callback(_string);
}

UciOption::UciOption(std::string initial, Callback<std::string> callback)
    : _type(kSTRING), _string(initial), _string_callback(callback)
{
    _string_callback(_string);
}

void UciOption::set(bool check)
{
    assert(_type == kCHECK);
    _check = check;
    _check_callback(_check);
}

void UciOption::set(int value)
{
    assert(_type == kSPIN);
    assert(_spin_min <= value && value <= _spin_max);
    _spin = value;
    _spin_callback(_spin);
}

void UciOption::set()
{
    assert(_type == kBUTTON);
    _button_callback();
}

void UciOption::set(std::string value)
{
    assert(_type == kCOMBO || _type == kSTRING);
    if (_type == kCOMBO)
        assert(std::find(_combo_options.begin(), _combo_options.end(), value) != _combo_options.end());
    _string = value;
    _string_callback(_string);
}

OptionType UciOption::get_type() const
{
    return _type;
}

}
