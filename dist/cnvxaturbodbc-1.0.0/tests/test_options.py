from turbodbc import Rows, make_options


def test_options_without_parameters():
    options = make_options()
    # one of the default parameters tested in the C++ part
    assert options.parameter_sets_to_buffer == 1000


def test_options_with_overrides():
    options = make_options(
        read_buffer_size=Rows(123),
        parameter_sets_to_buffer=2500,
        varchar_max_character_limit=42,
        prefer_unicode=True,
        use_async_io=True,
        autocommit=True,
        large_decimals_as_64_bit_types=True,
        limit_varchar_results_to_max=True,
        force_extra_capacity_for_unicode=True,
        fetch_wchar_as_char=True,
    )

    assert options.read_buffer_size.rows == 123
    assert options.parameter_sets_to_buffer == 2500
    assert options.varchar_max_character_limit == 42
    assert options.prefer_unicode is True
    assert options.use_async_io is True
    assert options.autocommit is True
    assert options.large_decimals_as_64_bit_types is True
    assert options.limit_varchar_results_to_max is True
    assert options.force_extra_capacity_for_unicode is True
    assert options.fetch_wchar_as_char is True
