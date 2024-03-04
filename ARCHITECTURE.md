[ft_irc](https://github.com/Taeil-Nam/ft_irc) 프로젝트의 전반적인 구조를 나타내는 문서이다.

## 프로젝트 요구사항
[프로젝트 요구사항 명세서](https://cdn.intra.42.fr/pdf/pdf/115395/en.subject.pdf)

## 코딩 컨벤션
[C++ 코딩 표준](https://docs.popekim.com/ko/coding-standards/cpp) 컨벤션을 기본으로 사용한다.
만약 기본 컨벤션에 해당하는 내용이 없는 경우, [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)를 참고하여 사용한다.

## 협업
### 소스코드 관리
[Project Repository (Github)](https://github.com/Taeil-Nam/ft_irc)
- Github의 repository를 사용하여 소스코드의 버전을 관리한다.
- ```main``` branch에는 적용 사항을 곧바로 적용할 수 없으며, 다른 branch에서 작업 후 pull request를 통해서만 변경 사항을 적용한다.
- Branch 네이밍 컨벤션은 아래와 같다:
```
* 기능 추가
- feature : 모든 팀원들이 사용하는 대표 브랜치
    -- [intra id]/feature/[클래스명] : 각 팀원마다 사용할 브랜치

* 로직 수정
- modify : 모든 팀원들이 사용하는 대표 브랜치
    -- [intra id]/modify/[클래스명] : 각 팀원마다 사용할 브랜치

* 리팩토링
- refactor : 모든 팀원들이 사용하는 대표 브랜치
    -- [intra id]/refactor/[클래스명] : 각 팀원마다 사용할 브랜치
...
```
- 다른 주제의 브랜치가 필요할 경우, 해당 브랜치를 새로 만들고 다른 팀원들에게 알리도록 한다.

### 코드 작성
다같이 얘기하면서 프로그래밍을 하는 기법인 ```짝 프로그래밍(Pair Programming)```을 지향한다.
어떠한 이유로 인해 개인별로 프로그래밍을 하게 되는 경우, 변경사항에 대해 팀원들에게 설명해야 한다.

## 구조
이 문서에서는 프로젝트를 구성하는 중요한 기능 또는 클래스와 같은 큰 범위의 구조만 나타낸다.
프로그램을 구성하는 클래스들이 어떤 메세지를 주고 받으며 구성되어있는지 나타낸다.
클래스의 상세 내용은 해당 소스코드를 참고한다.

### 주요 클래스
#### Core
- 서버의 모든 전반적인 로직을 수행한다.

#### Event
- fd에 대한 IO event를 등록하고 감시한다.

#### Network
- Network 통신과 관련된 로직을 수행한다.

#### ConsoleWindow
- 콘솔창 IO 역할을 수행한다.

#### GlobalLogger
- 특정 상황이나 상태를 로그 파일에 기록한다.

### 시각화
이미지 추가
