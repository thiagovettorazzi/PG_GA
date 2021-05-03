#include "SceneManager.h"

//static controllers for mouse and keyboard
static bool keys[1024];
static bool resized;
static GLuint width, height;

float tempoReferencial = (float)glfwGetTime();

// Variáveis relacionadas à mão
float maoComprimento = 360.0f;
float maoAltura = 316.0f;
float maoEixoY = 450.0f;

// Variáveis do game
int vidas = 3;
int pontos = 0;
int exibiuMensagemGameOver = 1;
int quantidadeAvioes = 20;

SceneManager::SceneManager() {}

SceneManager::~SceneManager() {}

void SceneManager::initialize(GLuint w, GLuint h) {
	width = w;
	height = h;
	
	// GLFW - GLEW - OPENGL general setup -- TODO: config file
	initializeGraphics();
}

void SceneManager::initializeGraphics() {
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(width, height, "Jogo do Silvio Santos", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Setando a callback de redimensionamento da janela
	glfwSetWindowSizeCallback(window, resize);
	
	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Build and compile our shader program
	addShader("../shaders/transformations.vs", "../shaders/transformations.frag");

	// CONFIGURAÇÃO
	setupScene();

	resized = true; // para entrar no setup da câmera na 1a vez
}

void SceneManager::addShader(string vFilename, string fFilename) {
	shader = new Shader (vFilename.c_str(), fFilename.c_str());
}


void SceneManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void SceneManager::resize(GLFWwindow * window, int w, int h) {
	width = w;
	height = h;
	resized = true;

	glViewport(0, 0, width, height);
}

void SceneManager::update() {
	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
		if (maoEixoY < 450.0f + maoAltura)
			maoEixoY += 0.5f;
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
		if (maoEixoY > maoAltura / 2)
			maoEixoY -= 0.5f;

	// Se o jogo pode continuar
	if (vidas > 0) {
		// Movimentação da mão
		objects[1]->setTranslation(glm::vec3(maoComprimento / 2, (float)maoEixoY, 0.0f), false);

		// Movimentação do avião
		if ((float)glfwGetTime() - tempoReferencial >= 0.001f) {
			for (int i = 2; i < quantidadeAvioes + 2; i++) {
				objects[i]->movimento();

				// Verifica a colisão do avião com o final da tela
				if (objects[i]->verificaColisaoComFinalTela()) {
					vidas -= 1;
					if (vidas > 0)
						cout << "Voce ainda tem " << vidas << " vidas" << endl;
				}

				// Verifica a colisão do avião com a mão
				if (objects[i]->verificaColisaoComAMao(maoAltura, maoComprimento, maoEixoY)) {
					pontos += 1;
					cout << "Voce tem: R$ " << pontos << ",00" << endl;
				}
			}
			tempoReferencial = (float)glfwGetTime();
		}
	}
	else {
		for (int i = 2; i < quantidadeAvioes + 2; i++) {
			objects[i]->setVivo(false);
		}
		if (exibiuMensagemGameOver == 1) {
			cout << "Game over!" << endl;
			exibiuMensagemGameOver -= 1;
		}
	}
}

void SceneManager::render() {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (resized) {
		setupCamera2D();
		resized = false;
	}

	// Atualiza e desenha os Sprites
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->update();
		objects[i]->draw();
	}
}

void SceneManager::run() {
	// GAME LOOP
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		update();
		render();
		glfwSwapBuffers(window);
	}
}

void SceneManager::finish() {
	glfwTerminate();
}


void SceneManager::setupScene() {
	// Adiciona o background
	Sprite* obj = new Sprite;
	obj->setPosition(glm::vec3(1050.0f, 450.0f, 0.0));
	obj->setDimension(glm::vec3(2100.0f, 900.0f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj);

	// Adiciona a mão
	obj = new Sprite;
	obj->setPosition(glm::vec3(maoComprimento / 2, 450.0f, 0.0));
	obj->setDimension(glm::vec3(maoComprimento, maoAltura, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj);

	// Carregamento das texturas do fundo e da mão
	unsigned int texID = loadTexture("../textures/background.jpg");
	objects[0]->setTexture(texID);

	texID = loadTexture("../textures/mao.png");
	objects[1]->setTexture(texID);

	texID = loadTexture("../textures/aviaozinho.png");

	// Adiciona os aviões
	int numeroAleatorio;

	for (int i = 0; i < quantidadeAvioes; i++) {
		numeroAleatorio = (rand() % 720) + 45;

		obj = new Sprite;
		obj->setPosition(glm::vec3(2400.0f + (i * 400), numeroAleatorio, 0.0));
		obj->setDimension(glm::vec3(200.0f, 90.0f, 1.0f));
		obj->setShader(shader);
		obj->setTexture(texID);
		objects.push_back(obj);
	}

	// Definindo a janela do mundo
	ortho2D[0] = 0.0f;
	ortho2D[1] = 2100.0f;
	ortho2D[2] = 0.0f;
	ortho2D[3] = 900.0f;

	// Habilita transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SceneManager::setupCamera2D() {
	float zNear = -1.0, zFar = 1.0;

	projection = glm::ortho(ortho2D[0], ortho2D[1], ortho2D[2], ortho2D[3], zNear, zFar);

	// Obtendo o identificador da matriz de projeção para enviar para o shader
	GLint projLoc = glGetUniformLocation(shader->ID, "projection");

	// Enviando a matriz de projeção para o shader
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

unsigned int SceneManager::loadTexture(string filename) {
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	
	if (data) {
		if (nrChannels == 3) { // jpg, bmp
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else { // png
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	return texture;
}
