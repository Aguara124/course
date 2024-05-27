#ifndef AUTHEXCEPTION_H
#define AUTHEXCEPTION_H

class AuthException : public std::exception {
 public:
  AuthException(const QString& message) : message{message} {}

  const char* what() const noexcept override {
    return message;
  }

 private:
  QString message;
};

#endif  // AUTHEXCEPTION_H
