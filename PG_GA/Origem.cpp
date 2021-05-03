using namespace std;

#include "SceneManager.h"

int main() {
	SceneManager* scene = new SceneManager;
	scene->initialize(2100, 900);

	scene->run();

	scene->finish();

	return 0;
}