#include "Sprite.h"

Sprite::Sprite() {
	initialize();
}

void Sprite::initialize() {
	float vertices[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	transform = glm::mat4(1); //matriz identidade
	texID = -1; //ainda não temos
	shader = NULL; //ainda não temos
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	angle = 0.0f;
	movimentacao = 0.0f;
	vivo = true;
}

void Sprite::setTexture(int texID) {
	this->texID = texID;
}

void Sprite::setRotation(float angle, glm::vec3 axis, bool reset) {
	if (reset) transform = glm::mat4(1);
	transform = glm::rotate(transform, angle, axis);
}

void Sprite::setTranslation(glm::vec3 displacements, bool reset) {
	pos = displacements;
	if (reset) transform = glm::mat4(1);
	transform = glm::translate(transform, displacements);
}

void Sprite::setScale(glm::vec3 scaleFactors, bool reset) {
	if (reset) transform = glm::mat4(1);
	transform = glm::scale(transform, scaleFactors);
	scale = scaleFactors;
}

void Sprite::movimento() {
	if (pos[0] > (scale.x / 2)) {
		movimentacao = pos[0] - 1.0f;
	}

	pos = glm::vec3(movimentacao, pos[1], 0.0f);
	setTranslation(pos);
}

void Sprite::setVivo(bool vivo) {
	vivo = vivo;
}

bool Sprite::verificaColisaoComAMao(float alturaMao, float comprimentoMao, float posicaoMaoY) {
	// Só verifica a colisão se está vivo
	if (vivo) {
		// Se bateu na mão em X
		if (pos.x - (scale.x / 2) < comprimentoMao - (comprimentoMao / 6)) { // Com uma margenzinha para o avião "entrar" na mão
			// Se bateu na mão em Y
			if (pos.y < posicaoMaoY + (alturaMao / 2) && pos.y > posicaoMaoY - (alturaMao / 2)) {
				vivo = false;
				return true;
			}

			return false;
		}

		return false;
	}

	return false;
}

bool Sprite::verificaColisaoComFinalTela() {
	// Só verifica a colisão se está vivo
	if (vivo) {
		// Se bateu na mão em X
		if (pos[0] == (scale.x / 2)) {
			vivo = false;
			return true;
		}

		return false;
	}
	return false;
}

void Sprite::draw() {
	// Só desenha se o avião está "vivo"
	if (vivo) {
		glBindTexture(GL_TEXTURE_2D, texID);
		glUniform1i(glGetUniformLocation(shader->ID, "ourTexture1"), 0);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Sprite::update() {
	setTranslation(pos);
	setRotation(angle, glm::vec3(0.0f, 0.0f, 1.0f), false);
	setScale(scale, false);

	GLint transformLoc = glGetUniformLocation(shader->ID, "model");
	// Pass them to the shaders
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
}
