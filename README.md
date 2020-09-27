# About Project
* This project represents a simple dictionary.
* Features:
    * Languages could be added:
        * Languages are represented as an ASCII string and are sorted by an English alphabet.
        * Languages could be represented as ASCII characters, numbers, and underscore sign.
        * Languages could be removed in the future by the corresponding button.
        * No duplicate names for languages are allowed(error message dialog will popup in such case).
    * Words could be added to each language:
        * Could be searched in the search bar(search requires 100% correspondence with the word, including the case, to work properly):
        * Could also be changed in the future or removed at all.
        * No duplicate words are allowed(error message dialog will popup in such a case).
* GUI is implemented with GTK3.
* Was written on Manjaro Linux.


# Layout:
* __Language list:__
    * Located on the lower-left part of the screen.
    * List is sorted by the names of the languages.
    * There is no UTF8 support, so in order for this sorting to work, languages should be represented in plain English(simple ASCII encoding).
* __New language add entry:__
    * Located on the top-left part of the screen.
    * Language is added by writing the name of the language(in English, as mentioned above) and pressing __add__ ![add](./Resources/add_language.png) _button_.
* __Words list:__
    * Located on the lower-left part of the screen.
    * To list the words of any language, simply press on the language button.
    * Every time a language button is pressed, the before chosen language's words are written to its corresponding file and newly chosen language's words are added to the words list layout.
    * Each row in the words list consists of the word, definition, and __change__ ![change](./Resources/change.png) and __remove__ ![remove](./Resources/remove.png) _buttons_.
    * If change is made to the word and new word equals to the old word then simply the definition will change. In other cases the change of word corresponds to the rules of adding a word(see below) with only difference that after change the position of the word in the list is immutable, whereas after the addition of new word, the word is added to the end of the list.
* __New word add entry:__
    * Located on the top of the words list.
    * Contains of the new word and new definition entries, and of __add__ ![add](./Resources/add_language.png) _button_.
    * If word or defintion is not valid(contains '-', '\n', or is empty) the message dialog with corresponding error will be returned.
    * If active language is not set, it won't add the new word, as no language to add this word to is chosen. Message dialog with corresponding error string will be returned.
    * If such word already exists, the program will simply scroll to the word in the list and return message dialog with corresponding error string.
* __Search:__
    * Located on the top-left corner of the screen.
    * Contains of the search entry and __search__ ![search](./Resources/search.png) _button_.
    * It requires 100% correspondence with word, otherwise, it won't find it.
* __Active language:__
    * Located to the right of new language entry.
    * Shows currently chosen language.