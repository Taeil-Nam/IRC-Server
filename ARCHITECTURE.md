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
```짝 프로그래밍(Pair Programming)```을 지향한다.  
최대한 같이 모여 토론하면서 코드를 작성하도록 한다.  
어떠한 이유로 인해 개인별로 프로그래밍을 하게 되는 경우, 변경사항에 대해 팀원들에게 설명해야 한다.  

## 구조
이 문서에서는 프로젝트를 구성하는 중요한 **서브시스템**들의 구조를 나타낸다.  
각 서브시스템은 서브시스템 내의 **클래스** 구조를 나타낸다.  
이 문서에 명시된 구조를 참고하여 소프트웨어를 구현한다.  
클래스의 상세 내용은 해당 소스코드를 참고한다.  

<!-- - Network, UI, IRC Core, Logging 등 큰 서브시스템/패키지의 시각화 필요. -->

### UI
<!-- - UI 로직에 사용되는 클래스들의 시각화 필요. -->

### Network
<!-- - Network 로직에 사용되는 클래스들의 시각화 필요. -->

### IRC Core
<!-- - IRC Core 로직에 사용되는 클래스들의 시각화 필요. -->

### Logging
<!-- - Logging 로직에 사용되는 클래스들의 시각화 필요. -->

