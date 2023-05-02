#include <gtest/gtest.h>
#include <myers.h>
#include <string>
#include <string_view>

static void check(
    const std::string strx,
    const std::string stry,
    const std::string expected) {

  class MyersStringCtx : public MyersContext {
    const std::string &_strx;
    const std::string &_stry;

  public:
    MyersStringCtx(const std::string &strx,
                   const std::string &stry) : MyersContext(strx.size(), stry.size()), _strx(strx), _stry(stry) {
      // NOP
    }
    bool equals(const size_t posx, const size_t posy) {
      return _strx[posx] == _stry[posy];
    }
  };
  std::stringstream out;
  std::vector<Diff> summary;
  MyersStringCtx(strx, stry).diff(summary);
  for (Diff diff : summary) {
    switch (diff.op) {
    case BOTH:
      for (size_t i = diff.begx; i < diff.endx; i++)
        out << "=" << strx[i];
      break;
    case FILEX:
      for (size_t i = diff.begx; i < diff.endx; i++)
        out << "-" << strx[i];
      break;
    case FILEY:
      for (size_t i = diff.begy; i < diff.endy; i++)
        out << "+" << stry[i];
      break;
    }
  }
  ASSERT_STREQ(out.str().data(), expected.data());
}

TEST(MyersTests, TestEqEven) {
  check("aa", "aa", "=a=a");
}

TEST(MyersTests, TestEqOdd) {
  check("aaa", "aaa", "=a=a=a");
}

TEST(MyersTests, TestRemoveHeadEvenSizeEven) {
  check("abcd", "cd", "-a-b=c=d");
}

TEST(MyersTests, TestRemoveHeadOddSizeEven) {
  check("abcd", "d", "-a-b-c=d");
}

TEST(MyersTests, TestRemoveHeadEvenSizeOdd) {
  check("abcde", "cde", "-a-b=c=d=e");
}

TEST(MyersTests, TestRemoveHeadOddSizeOdd) {
  check("abcde", "de", "-a-b-c=d=e");
}

TEST(MyersTests, TestAppendHeadEvenSizeEven) {
  check("cd", "abcd", "+a+b=c=d");
}

TEST(MyersTests, TestAppendHeadOddSizeEven) {
  check("d", "abcd", "+a+b+c=d");
}

TEST(MyersTests, TestAppendHeadEvenSizeOdd) {
  check("cde", "abcde", "+a+b=c=d=e");
}

TEST(MyersTests, TestAppendHeadOddSizeOdd) {
  check("de", "abcde", "+a+b+c=d=e");
}

TEST(MyersTests, TestRemoveTailEvenSizeEven) {
  check("abcd", "ab", "=a=b-c-d");
}

TEST(MyersTests, TestRemoveTailOddSizeEven) {
  check("abcd", "a", "=a-b-c-d");
}

TEST(MyersTests, TestRemoveTailEvenSizeOdd) {
  check("abcde", "ab", "=a=b-c-d-e");
}

TEST(MyersTests, TestRemoveTailOddSizeOdd) {
  check("abcde", "abc", "=a=b=c-d-e");
}

TEST(MyersTests, TestAppendTailEvenSizeEven) {
  check("ab", "abcd", "=a=b+c+d");
}

TEST(MyersTests, TestAppendTailOddSizeEven) {
  check("a", "abcd", "=a+b+c+d");
}

TEST(MyersTests, TestAppendTailEvenSizeOdd) {
  check("abc", "abcde", "=a=b=c+d+e");
}

TEST(MyersTests, TestAppendTailOddSizeOdd) {
  check("ab", "abcde", "=a=b+c+d+e");
}

TEST(MyersTests, TestRemoveMidEvenSizeEven) {
  check("abcd", "ad", "=a-b-c=d");
}

TEST(MyersTests, TestRemoveMidOddSizeEven) {
  check("abcd", "abd", "=a=b-c=d");
}

TEST(MyersTests, TestRemoveMidEvenSizeOdd) {
  check("abcde", "abe", "=a=b-c-d=e");
}

TEST(MyersTests, TestRemoveMidOddSizeOdd) {
  check("abcde", "abde", "=a=b-c=d=e");
}

TEST(MyersTests, TestAppendMidEvenSizeEven) {
  check("ad", "abcd", "=a+b+c=d");
}

TEST(MyersTests, TestAppendMidOddSizeEven) {
  check("abd", "abcd", "=a=b+c=d");
}

TEST(MyersTests, TestAppendMidEvenSizeOdd) {
  check("ade", "abcde", "=a+b+c=d=e");
}

TEST(MyersTests, TestAppendMidOddSizeOdd) {
  check("ae", "abcde", "=a+b+c+d=e");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
