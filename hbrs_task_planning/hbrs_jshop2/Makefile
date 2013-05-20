all: installed

GIT_DIR=build/jshop2
GIT_URL=https://github.com/b-it-bots/jshop2.git
include $(shell rospack find mk)/git_checkout.mk


installed: $(GIT_DIR) patched
	install -dm755 "$(GIT_DIR)/bin.build"
	export CLASSPATH="${CLASSPATH}":"$(GIT_DIR)/":"$(GIT_DIR)/antlr.jar"
	make -C "$(GIT_DIR)/"
	
	install -dm755 lib
	install -Dm644 "$(GIT_DIR)/bin.build/JSHOP2.jar" lib/
	
	touch installed

clean:
	make -C "$(GIT_DIR)/" clean
	rm -rf installed patched

wipe: clean
	rm -rf $(GIT_DIR) build rospack_nosubdirs
