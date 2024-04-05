# bot

Телеграм бот с использованием библиотеки POCO

 * [Метод](https://core.telegram.org/bots/api#getme) `/getMe`
   возвращает информацию о вашем боте. Его удобно использовать для
   проверки токена.

```bash
$ curl https://api.telegram.org/bot<YOUR_TOKEN_HERE>/getMe
{"ok":true,"result":{"id":384306257,"is_bot":true,"first_name":"shad-cpp-test","username":"shad_shad_test_test_bot"}}
```

 * [Метод](https://core.telegram.org/bots/api#getting-updates)
   `/getUpdates` возвращает сообщения, направленные боту

```bash
$ curl -s https://api.telegram.org/bot<YOUR_TOKEN_HERE>/getUpdates | json_pp
{
   "ok" : true,
   "result" : [
      {
         "message" : {
            "date" : 1510493105,
            "entities" : [
               {
                  "length" : 6,
                  "offset" : 0,
                  "type" : "bot_command"
               }
            ],
            "from" : {
               "is_bot" : false,
               "username" : "darth_slon",
               "id" : 104519755,
               "first_name" : "Fedor"
            },
            "chat" : {
               "username" : "darth_slon",
               "type" : "private",
               "first_name" : "Fedor",
               "id" : 104519755
            },
            "text" : "/start",
            "message_id" : 1
         },
         "update_id" : 851793506
      }
   ]
}
```

 * [Метод](https://core.telegram.org/bots/api#sendmessage)
   `/sendMessage` позволяет послать сообщение

```bash
$ curl -s -H "Content-Type: application/json" -X POST -d '{"chat_id": <CHAT_ID_FROM_GET_UPDATES>, "text": "Hi!"}' https://api.telegram.org/bot<YOUR_TOKEN_HERE>/sendMessage | json_pp
{
   "ok" : true,
   "result" : {
      "chat" : {
         "type" : "private",
         "id" : 104519755,
         "username" : "darth_slon",
         "first_name" : "Fedor"
      },
      "message_id" : 5,
      "from" : {
         "is_bot" : true,
         "first_name" : "shad-cpp-test",
         "id" : 384306257,
         "username" : "shad_shad_test_test_bot"
      },
      "date" : 1510500325,
      "text" : "Hi!"
   }
}
```


## Особенности клиента
 * `Poco::Json` нигде не торчит из интерфейса.

 * Ошибки HTTP-API транслируются в исключения.

 * Клиент сохраняет текущий offset в файл и восстанавливает его после перезапуска.

 * Клиент ничего не знает про логику конкретного бота.

## Что делает бот

 * Запрос `/random`. Бот посылает случайное число ответом на это сообщение.

 * Запрос `/weather`. Бот отвечает в чат `Winter Is Coming`.

 * Запрос `/styleguide`. Бот отвечает в чат смешной шуткой на тему code review.

 * Запрос `/stop`. Процесс бота завершается штатно.

 * Запрос `/crash`. Процесс бота завершается аварийно. Например выполняет `std::abort();`.

## Автоматические тесты

Для тестирование используется `FakeServer` и написанные функциональные тесты, проверяющие набор сценариев.
`FakeServer` только прикидывается сервером и отвечает на все запросы заранее заготовленными ответами.
