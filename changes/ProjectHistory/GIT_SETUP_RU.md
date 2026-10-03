# Toy OS v65.15 — Git: инструкция для Windows 7 / W64DevKit

Этот пакет подготовлен как Git-готовая копия стабильной ветки Toy OS v65.15.
Рабочая логика исходников и структура проекта сохранены; добавлены только файлы
Git-обвязки `.gitignore`, `.gitattributes`, `GIT_SETUP_RU.md`, `git_publish.bat`
и `git_publish.sh`.

## 1. Что нужно на Windows 7

Нужен установленный Git, доступный из `cmd.exe` или из W64DevKit shell:

```text
git --version
```

Проверьте также имя и e-mail автора:

```text
git config --global user.name
 git config --global user.email
```

Если значения не заданы, задайте их самостоятельно:

```text
git config --global user.name "ВАШЕ ИМЯ"
git config --global user.email "ВАШ_EMAIL"
```

Не помещайте пароль или Personal Access Token в этот проект, BAT-файлы или Git-историю.
Для HTTPS используйте механизм учётных данных Git; для SSH — собственный SSH-ключ.

## 2. Первый локальный commit

Откройте W64DevKit shell в корне проекта:

```sh
git init
git add .
git status
git commit -m "Toy OS v65.15"
git branch -M main
```

`git status` перед commit позволяет убедиться, что в индекс не попали временные
файлы сборки или личные данные.

## 3. Подключение GitHub/GitLab/другого сервера

Создайте пустой удалённый репозиторий, затем:

```sh
git remote add origin <URL_ВАШЕГО_РЕПОЗИТОРИЯ>
git push -u origin main
```

Для GitHub это может выглядеть так:

```sh
git remote add origin https://github.com/<ВАШ_ЛОГИН>/ToyOS.git
git push -u origin main
```

## 4. Зафиксировать стабильную версию

После первого commit:

```sh
git tag -a v65.15 -m "Toy OS v65.15"
git push origin v65.15
```

## 5. Удобный вариант через BAT

Из `cmd.exe` можно выполнить:

```bat
git_publish.bat https://github.com/<ВАШ_ЛОГИН>/ToyOS.git
```

Скрипт проверит Git, имя автора, создаст `main`, добавит файлы, создаст commit
`Toy OS v65.15`, создаст тег `v65.15`, добавит `origin` и выполнит push.

Если `origin` уже существует, скрипт не заменяет его автоматически и завершает
работу с пояснением — это защита от случайной публикации в неправильный репозиторий.

## 6. Что будет храниться в Git

В Git входят исходники ядра, Ring-3 программ, сетевого драйвера, VGA-драйвера,
FAT16-утилит, PNG2RAW, тестов, документации и ресурсов проекта.

Каталог `build/` не хранится в Git: он создаётся командой сборки заново.
Так же не хранится сгенерированный `tools/PNG2RAW.EXE`.

`resources/SPLASH.RAW`, `resources/SPLASH_PREVIEW.png` и `resources/AUTOSTART.SH`
являются частью проекта и поэтому остаются под контролем версий.

## 7. Сборка после clone

В W64DevKit shell:

```sh
./build.sh
```

Или из `cmd.exe`, если скрипт запускается через W64DevKit:

```bat
build.bat
```

Ожидаемый результат — успешная сборка `build/toy_os.img` и прохождение встроенной
серии проверок.
