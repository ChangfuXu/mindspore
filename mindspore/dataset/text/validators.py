# Copyright 2019 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""
validators for text ops
"""

from functools import wraps
import mindspore.common.dtype as mstype

import mindspore._c_dataengine as cde
from mindspore._c_expression import typing

from ..core.validator_helpers import parse_user_args, type_check, type_check_list, check_uint32, \
    INT32_MAX, check_value, check_positive


def check_unique_list_of_words(words, arg_name):
    """Check that words is a list and each element is a str without any duplication"""

    type_check(words, (list,), arg_name)
    words_set = set()
    for word in words:
        type_check(word, (str,), arg_name)
        if word in words_set:
            raise ValueError(arg_name + " contains duplicate word: " + word + ".")
        words_set.add(word)
    return words_set


def check_lookup(method):
    """A wrapper that wraps a parameter checker to the original function."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [vocab, unknown_token], _ = parse_user_args(method, *args, **kwargs)

        if unknown_token is not None:
            type_check(unknown_token, (str,), "unknown_token")

        type_check(vocab, (cde.Vocab,), "vocab is not an instance of cde.Vocab.")

        return method(self, *args, **kwargs)

    return new_method


def check_from_file(method):
    """A wrapper that wraps a parameter checker to the original function."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [file_path, delimiter, vocab_size, special_tokens, special_first], _ = parse_user_args(method, *args,
                                                                                               **kwargs)
        if special_tokens is not None:
            check_unique_list_of_words(special_tokens, "special_tokens")
        type_check_list([file_path, delimiter], (str,), ["file_path", "delimiter"])
        if vocab_size is not None:
            check_value(vocab_size, (-1, INT32_MAX), "vocab_size")
        type_check(special_first, (bool,), special_first)

        return method(self, *args, **kwargs)

    return new_method


def check_from_list(method):
    """A wrapper that wraps a parameter checker to the original function."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [word_list, special_tokens, special_first], _ = parse_user_args(method, *args, **kwargs)

        word_set = check_unique_list_of_words(word_list, "word_list")
        if special_tokens is not None:
            token_set = check_unique_list_of_words(special_tokens, "special_tokens")

            intersect = word_set.intersection(token_set)

            if intersect != set():
                raise ValueError("special_tokens and word_list contain duplicate word :" + str(intersect) + ".")

        type_check(special_first, (bool,), "special_first")

        return method(self, *args, **kwargs)

    return new_method


def check_from_dict(method):
    """A wrapper that wraps a parameter checker to the original function."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [word_dict], _ = parse_user_args(method, *args, **kwargs)

        type_check(word_dict, (dict,), "word_dict")

        for word, word_id in word_dict.items():
            type_check(word, (str,), "word")
            type_check(word_id, (int,), "word_id")
            check_value(word_id, (0, INT32_MAX), "word_id")
        return method(self, *args, **kwargs)

    return new_method


def check_jieba_init(method):
    """Wrapper method to check the parameters of jieba init."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [hmm_path, mp_path, _, with_offsets], _ = parse_user_args(method, *args, **kwargs)

        if hmm_path is None:
            raise ValueError("The dict of HMMSegment in cppjieba is not provided.")
        if not isinstance(hmm_path, str):
            raise TypeError("Wrong input type for hmm_path, should be string.")
        if mp_path is None:
            raise ValueError("The dict of MPSegment in cppjieba is not provided.")
        if not isinstance(mp_path, str):
            raise TypeError("Wrong input type for mp_path, should be string.")
        if not isinstance(with_offsets, bool):
            raise TypeError("Wrong input type for with_offsets, should be boolean.")
        return method(self, *args, **kwargs)

    return new_method


def check_jieba_add_word(method):
    """Wrapper method to check the parameters of jieba add word."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [word, freq], _ = parse_user_args(method, *args, **kwargs)
        if word is None:
            raise ValueError("word is not provided.")
        if freq is not None:
            check_uint32(freq)
        return method(self, *args, **kwargs)

    return new_method


def check_jieba_add_dict(method):
    """Wrapper method to check the parameters of add dict."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        parse_user_args(method, *args, **kwargs)
        return method(self, *args, **kwargs)

    return new_method


def check_with_offsets(method):
    """Wrapper method to check if with_offsets is the only one parameter."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [with_offsets], _ = parse_user_args(method, *args, **kwargs)
        if not isinstance(with_offsets, bool):
            raise TypeError("Wrong input type for with_offsets, should be boolean.")
        return method(self, *args, **kwargs)

    return new_method


def check_unicode_script_tokenizer(method):
    """Wrapper method to check the parameter of UnicodeScriptTokenizer."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [keep_whitespace, with_offsets], _ = parse_user_args(method, *args, **kwargs)
        if not isinstance(keep_whitespace, bool):
            raise TypeError("Wrong input type for keep_whitespace, should be boolean.")
        if not isinstance(with_offsets, bool):
            raise TypeError("Wrong input type for with_offsets, should be boolean.")
        return method(self, *args, **kwargs)

    return new_method


def check_wordpiece_tokenizer(method):
    """Wrapper method to check the parameter of WordpieceTokenizer."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [vocab, suffix_indicator, max_bytes_per_token, unknown_token, with_offsets], _ = \
            parse_user_args(method, *args, **kwargs)
        if vocab is None:
            raise ValueError("vocab is not provided.")
        if not isinstance(vocab, cde.Vocab):
            raise TypeError("Wrong input type for vocab, should be Vocab object.")
        if not isinstance(suffix_indicator, str):
            raise TypeError("Wrong input type for suffix_indicator, should be string.")
        if not isinstance(unknown_token, str):
            raise TypeError("Wrong input type for unknown_token, should be string.")
        if not isinstance(with_offsets, bool):
            raise TypeError("Wrong input type for with_offsets, should be boolean.")
        check_uint32(max_bytes_per_token)
        return method(self, *args, **kwargs)

    return new_method


def check_regex_tokenizer(method):
    """Wrapper method to check the parameter of RegexTokenizer."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [delim_pattern, keep_delim_pattern, with_offsets], _ = parse_user_args(method, *args, **kwargs)
        if delim_pattern is None:
            raise ValueError("delim_pattern is not provided.")
        if not isinstance(delim_pattern, str):
            raise TypeError("Wrong input type for delim_pattern, should be string.")
        if not isinstance(keep_delim_pattern, str):
            raise TypeError("Wrong input type for keep_delim_pattern, should be string.")
        if not isinstance(with_offsets, bool):
            raise TypeError("Wrong input type for with_offsets, should be boolean.")
        return method(self, *args, **kwargs)

    return new_method


def check_basic_tokenizer(method):
    """Wrapper method to check the parameter of RegexTokenizer."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [lower_case, keep_whitespace, _, preserve_unused, with_offsets], _ = \
            parse_user_args(method, *args, **kwargs)
        if not isinstance(lower_case, bool):
            raise TypeError("Wrong input type for lower_case, should be boolean.")
        if not isinstance(keep_whitespace, bool):
            raise TypeError("Wrong input type for keep_whitespace, should be boolean.")
        if not isinstance(preserve_unused, bool):
            raise TypeError("Wrong input type for preserve_unused_token, should be boolean.")
        if not isinstance(with_offsets, bool):
            raise TypeError("Wrong input type for with_offsets, should be boolean.")
        return method(self, *args, **kwargs)

    return new_method


def check_bert_tokenizer(method):
    """Wrapper method to check the parameter of BertTokenizer."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [vocab, suffix_indicator, max_bytes_per_token, unknown_token, lower_case, keep_whitespace, _,
         preserve_unused_token, with_offsets], _ = parse_user_args(method, *args, **kwargs)
        if vocab is None:
            raise ValueError("vacab is not provided.")
        if not isinstance(vocab, cde.Vocab):
            raise TypeError("Wrong input type for vocab, should be Vocab object.")
        if not isinstance(suffix_indicator, str):
            raise TypeError("Wrong input type for suffix_indicator, should be string.")
        if not isinstance(max_bytes_per_token, int):
            raise TypeError("Wrong input type for max_bytes_per_token, should be int.")
        check_uint32(max_bytes_per_token)

        if not isinstance(unknown_token, str):
            raise TypeError("Wrong input type for unknown_token, should be string.")
        if not isinstance(lower_case, bool):
            raise TypeError("Wrong input type for lower_case, should be boolean.")
        if not isinstance(keep_whitespace, bool):
            raise TypeError("Wrong input type for keep_whitespace, should be boolean.")
        if not isinstance(preserve_unused_token, bool):
            raise TypeError("Wrong input type for preserve_unused_token, should be boolean.")
        if not isinstance(with_offsets, bool):
            raise TypeError("Wrong input type for with_offsets, should be boolean.")
        return method(self, *args, **kwargs)

    return new_method


def check_from_dataset(method):
    """A wrapper that wraps a parameter checker to the original function."""

    @wraps(method)
    def new_method(self, *args, **kwargs):

        [_, columns, freq_range, top_k, special_tokens, special_first], _ = parse_user_args(method, *args,
                                                                                            **kwargs)
        if columns is not None:
            if not isinstance(columns, list):
                columns = [columns]
                col_names = ["col_{0}".format(i) for i in range(len(columns))]
                type_check_list(columns, (str,), col_names)

        if freq_range is not None:
            type_check(freq_range, (tuple,), "freq_range")

            if len(freq_range) != 2:
                raise ValueError("freq_range needs to be a tuple of 2 integers or an int and a None.")

            for num in freq_range:
                if num is not None and (not isinstance(num, int)):
                    raise ValueError(
                        "freq_range needs to be either None or a tuple of 2 integers or an int and a None.")

            if isinstance(freq_range[0], int) and isinstance(freq_range[1], int):
                if freq_range[0] > freq_range[1] or freq_range[0] < 0:
                    raise ValueError("frequency range [a,b] should be 0 <= a <= b (a,b are inclusive).")

        type_check(top_k, (int, type(None)), "top_k")

        if isinstance(top_k, int):
            check_positive(top_k, "top_k")
        type_check(special_first, (bool,), "special_first")

        if special_tokens is not None:
            check_unique_list_of_words(special_tokens, "special_tokens")

        return method(self, *args, **kwargs)

    return new_method


def check_ngram(method):
    """A wrapper that wraps a parameter checker to the original function."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [n, left_pad, right_pad, separator], _ = parse_user_args(method, *args, **kwargs)

        if isinstance(n, int):
            n = [n]

        if not (isinstance(n, list) and n != []):
            raise ValueError("n needs to be a non-empty list of positive integers.")

        for i, gram in enumerate(n):
            type_check(gram, (int,), "gram[{0}]".format(i))
            check_positive(gram, "gram_{}".format(i))

        if not (isinstance(left_pad, tuple) and len(left_pad) == 2 and isinstance(left_pad[0], str) and isinstance(
                left_pad[1], int)):
            raise ValueError("left_pad needs to be a tuple of (str, int) str is pad token and int is pad_width.")

        if not (isinstance(right_pad, tuple) and len(right_pad) == 2 and isinstance(right_pad[0], str) and isinstance(
                right_pad[1], int)):
            raise ValueError("right_pad needs to be a tuple of (str, int) str is pad token and int is pad_width.")

        if not (left_pad[1] >= 0 and right_pad[1] >= 0):
            raise ValueError("padding width need to be positive numbers.")

        type_check(separator, (str,), "separator")

        kwargs["n"] = n
        kwargs["left_pad"] = left_pad
        kwargs["right_pad"] = right_pad
        kwargs["separator"] = separator

        return method(self, **kwargs)

    return new_method


def check_pair_truncate(method):
    """Wrapper method to check the parameters of number of pair truncate."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        parse_user_args(method, *args, **kwargs)
        return method(self, *args, **kwargs)

    return new_method


def check_to_number(method):
    """A wrapper that wraps a parameter check to the original function (ToNumber)."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [data_type], _ = parse_user_args(method, *args, **kwargs)
        type_check(data_type, (typing.Type,), "data_type")

        if data_type not in mstype.number_type:
            raise TypeError("data_type is not numeric data type.")

        return method(self, *args, **kwargs)

    return new_method


def check_python_tokenizer(method):
    """A wrapper that wraps a parameter check to the original function (PythonTokenizer)."""

    @wraps(method)
    def new_method(self, *args, **kwargs):
        [tokenizer], _ = parse_user_args(method, *args, **kwargs)

        if not callable(tokenizer):
            raise TypeError("tokenizer is not a callable python function")

        return method(self, *args, **kwargs)

    return new_method
