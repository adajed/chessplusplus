#ifndef CHESS_ENGINE_UCI_OPTION_H_
#define CHESS_ENGINE_UCI_OPTION_H_

#include <functional>
#include <string>
#include <vector>

namespace engine
{

using CallbackVoid = std::function<void()>;

template <typename Type>
using Callback = std::function<void(Type)>;

enum OptionType
{
    kCHECK,
    kSPIN,
    kCOMBO,
    kBUTTON,
    kSTRING
};

inline std::string optiontype_to_string(OptionType optiontype)
{
    switch (optiontype)
    {
    case kCHECK:  return "check";
    case kSPIN:   return "spin";
    case kCOMBO:  return "combo";
    case kBUTTON: return "button";
    case kSTRING: return "string";
    }

    return "";
};

class UciOption
{
    public:

        UciOption() = default;

        // check type
        UciOption(bool check, Callback<bool> callback);

        // spin type
        UciOption(int inital, int min, int max, Callback<int> callback);

        // combo type
        UciOption(std::string initial, std::vector<std::string> options, Callback<std::string> callback);

        // button type
        UciOption(CallbackVoid callback);

        // string type
        UciOption(std::string initial, Callback<std::string> callback);

        UciOption(const UciOption& other) = default;
        UciOption& operator= (const UciOption& other) = default;

        bool get_check() const { return _check; }

        int get_spin_initial() const { return _spin; }

        int get_spin_min() const { return _spin_min; }

        int get_spin_max() const { return _spin_max; }

        std::vector<std::string> get_combo_options() const { return _combo_options; }

        std::string get_string() const { return _string; }

        void set(bool check);

        void set(int value);

        void set();

        void set(std::string value);

        OptionType get_type() const;

    public:

        OptionType _type;

        bool _check;
        int _spin;
        int _spin_min, _spin_max;
        std::vector<std::string> _combo_options;
        std::string _string; // used both in combo and string

        Callback<bool> _check_callback;
        Callback<int> _spin_callback;
        CallbackVoid _button_callback;
        Callback<std::string> _string_callback; // used both in combo and string
};

};

#endif  // CHESS_ENGINE_UCI_OPTION_H_
