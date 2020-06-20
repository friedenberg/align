
void testRemoveTrailingNewlines(void) {
  char *original = "worm\n";
  TEST_ASSERT_EQUAL_STRING_MESSAGE(
      "worm",
      remove_trailing_newline(original),
      "string should have no trailing newline"
      );
}
