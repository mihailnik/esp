#include <SPIFFSEditor.h>

AsyncWebServer server(80);

void page_custom(void);

void html_pages()
{
  p_index_html();
  page_update();
  title_update();
  menu_update();
  page_support();
  page_post();
  reboot_page();
  page_custom();
    }

    void page_update()
    {
      server.on("/echo", HTTP_ANY, [](AsyncWebServerRequest *request) { // сюда JS стучится за данными для БЛАНКА
        int page = request->arg("p").toInt();
        current_page = page;
        DEBG(String(F("Page num: ")) + String(page));
        make_page(page); // Получение контента для страницы, по номеру страницы
        request->send(200, F("text/plain"), page_content);
        page_content = "";
        DEBG("RAM: " + String(ESP.getFreeHeap()));
      });
    }

    void title_update()
    {
      server.on("/title", HTTP_ANY, [](AsyncWebServerRequest *request) { // сюда JS стучится за данными для БЛАНКА
        int page = request->arg("p").toInt();
        request->send(200, F("text/plain"), e_menu[page]);
      });
    }

    void menu_update()
    {
      server.on("/menu", HTTP_ANY, [](AsyncWebServerRequest *request) { // сюда JS стучится за данными для БЛАНКА
        int page = request->arg("p").toInt();
        make_menu(page);
        request->send(200, F("text/plain"), menu_content);
        menu_content = "";
      });
    }

    void reboot_page()
    {
      server.on("/reboot", HTTP_ANY, [](AsyncWebServerRequest *request) {
        request->send(200, F("text/plain"), "OK");
        save_flag = true;
        save_param();
        ESP.restart();
      });
    }

    void page_custom()
    {
      server.on("/custom", HTTP_ANY, [](AsyncWebServerRequest *request) {
        int params = request->params();
        AsyncWebParameter *p;
        for (int i = 0; i < params; i++)
        {
          p = request->getParam(i);
          DEBG(String(p->name() + ": " + String(p->value())));
          if (p->name() == "play")
          {
            mp3_setup(p->value());
          }
            
          if (p->name() == "del")
          {
            SPIFFS.remove(p->value());
            request->redirect("/");
          }
            
        }
        
        request->send(200, F("text/plain"), F("test!"));
      });
    }

    void page_post()
    {
      server.on("/post", HTTP_ANY, [](AsyncWebServerRequest *request) {
        int params = request->params();
        AsyncWebParameter *p;
        for (int i = 0; i < params; i++)
        {
          p = request->getParam(i);
          write_param(p->name(), p->value());
          DEBG(String(p->name() + ": " + String(p->value())));
        }
        request->send(200, F("text/plain"), F("Настройки сохранены!"));
        save_flag = true;
        gpio_setup();
      });
    }

    void p_index_html()
    {
      server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/html"), template_default, template_default_len);
        response->addHeader(F("Content-Encoding"), F("gzip"));
        request->send(response);
      });
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  struct uploadRequest
  {
    uploadRequest *next;
    AsyncWebServerRequest *request;
    File uploadFile;
    uint32_t fileSize;
    uploadRequest()
    {
      next = NULL;
      request = NULL;
      fileSize = 0;
    }
  };
  static uploadRequest uploadRequestHead;
  uploadRequest *thisUploadRequest = NULL;

  if (!index)
  {
    String toFile = filename;
    if (request->hasParam("dir", true))
    {
      AsyncWebParameter *p = request->getParam("dir", true);
      DBG_OUTPUT_PORT.println("dir param: " + p->value());
      toFile = p->value();
      if (!toFile.endsWith("/"))
        toFile += "/";
      toFile += filename;
    }
    if (!toFile.startsWith("/"))
      toFile = "/" + toFile;

    if (SPIFFS.exists(toFile))
      SPIFFS.remove(toFile);
    thisUploadRequest = new uploadRequest;
    thisUploadRequest->request = request;
    thisUploadRequest->next = uploadRequestHead.next;
    uploadRequestHead.next = thisUploadRequest;
    thisUploadRequest->uploadFile = SPIFFS.open(toFile, "w");
    DBG_OUTPUT_PORT.println("Upload: START, filename: " + toFile);
  }
  else
  {
    thisUploadRequest = uploadRequestHead.next;
    while (thisUploadRequest->request != request)
      thisUploadRequest = thisUploadRequest->next;
  }

  if (thisUploadRequest->uploadFile)
  {
    for (size_t i = 0; i < len; i++)
    {
      thisUploadRequest->uploadFile.write(data[i]);
    }
    thisUploadRequest->fileSize += len;
  }

  if (final)
  {
    thisUploadRequest->uploadFile.close();
    DBG_OUTPUT_PORT.print("Upload: END, Size: ");
    DBG_OUTPUT_PORT.println(thisUploadRequest->fileSize);
    uploadRequest *linkUploadRequest = &uploadRequestHead;
    while (linkUploadRequest->next != thisUploadRequest)
      linkUploadRequest = linkUploadRequest->next;
    linkUploadRequest->next = thisUploadRequest->next;
    delete thisUploadRequest;
    request->redirect("/");
  }
}

const char *http_username = "admin";
const char *http_password = "admin";

void page_support(){

  server.on("/css/bootstrap.min.css", HTTP_ANY, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/plain"), bootstrap_css, bootstrap_css_len);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });

  server.on("/js/bubbly_bg.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), bubbly_bg_js, bubbly_bg_js_len);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });

  server.on("/images/logo.svg", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/svg+xml"), logo_svg, logo_svg_len);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });

  server.on("/favicon.ico", HTTP_ANY, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/svg+xml"), favicon, favicon_len);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });

  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200);
  },
            handleUpload);
  server.begin();
}