#include <iostream>
#include <fstream>
#include <string>
#include <direct.h>   // _mkdir
#include <sys/stat.h> // stat

using namespace std;

class Repository {
public:
    Repository() {}

    // ---------- INIT ----------
    void init() {
        if (!exists(".mygit")) {
            _mkdir(".mygit");
            createHead();
        } else {
            ifstream headFile(".mygit/HEAD");
            readHead(headFile);
        }
    }

    // ---------- STATUS ----------
    void status() {
        if (!exists(".mygit/HEAD")) {
            cout << "Not a mygit repository" << endl;
            return;
        }

        ifstream headFile(".mygit/HEAD");
        string content;
        getline(headFile, content);

        cout << "On branch " << content.substr(5) << endl;
        cout << "Staged files:" << endl;

        ifstream indexFile(".mygit/index");
        string file;
        bool empty = true;

        while (getline(indexFile, file)) {
            if (!file.empty()) {
                cout << "  " << file << endl;
                empty = false;
            }
        }

        if (empty) cout << "  (none)" << endl;
    }

    // ---------- ADD ----------
    void add(const string& filename) {
        if (!exists(filename.c_str())) {
            cout << "Error: file '" << filename << "' does not exist" << endl;
            return;
        }

        ifstream indexRead(".mygit/index");
        string line;
        while (getline(indexRead, line)) {
            if (line == filename) {
                cout << "File '" << filename << "' is already staged" << endl;
                return;
            }
        }

        ofstream indexWrite(".mygit/index", ios::app);
        indexWrite << filename << endl;
        cout << "Added " << filename << " to staging area" << endl;
    }

    // ---------- COMMIT ----------
    void commit(const string& message) {
        int commitId;
        ifstream(".mygit/COMMIT_COUNT") >> commitId;
        commitId++;

        _mkdir(".mygit/commits");
        string commitDir = ".mygit/commits/" + to_string(commitId);
        _mkdir(commitDir.c_str());

        ifstream indexFile(".mygit/index");
        string filename;

        while (getline(indexFile, filename)) {
            if (filename.empty()) continue;

            ifstream src(filename, ios::binary);
            ofstream dest(commitDir + "/" + filename, ios::binary);
            dest << src.rdbuf();
        }

        ofstream(commitDir + "/message.txt") << message;
        ofstream(".mygit/COMMIT_COUNT") << commitId;

        // clear staging
        ofstream(".mygit/index").close();

        cout << "Committed as commit #" << commitId << endl;
    }

    // ---------- LOG (NEW) ----------
    void log() {
        if (!exists(".mygit/COMMIT_COUNT")) {
            cout << "No commits yet" << endl;
            return;
        }

        int commitCount;
        ifstream(".mygit/COMMIT_COUNT") >> commitCount;

        if (commitCount == 0) {
            cout << "No commits yet" << endl;
            return;
        }

        for (int i = commitCount; i >= 1; i--) {
            string msgPath = ".mygit/commits/" + to_string(i) + "/message.txt";
            ifstream msgFile(msgPath);

            if (!msgFile) continue;

            string message;
            getline(msgFile, message);

            cout << "commit " << i << endl;
            cout << "    " << message << endl << endl;
        }
    }

private:
    // ---------- HELPERS ----------
    bool exists(const string& path) {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }

    void createHead() {
        ofstream(".mygit/HEAD") << "ref: master";
        ofstream(".mygit/index").close();
        _mkdir(".mygit/commits");
        ofstream(".mygit/COMMIT_COUNT") << 0;

        cout << "Repository initialized with HEAD pointing to master" << endl;
    }

    void readHead(ifstream& headFile) {
        string content;
        getline(headFile, content);
        cout << "Existing HEAD content: " << content << endl;
    }
};

// ---------- MAIN ----------
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: mygit init | status | add <file> | commit <msg> | log" << endl;
        return 0;
    }

    Repository repo;
    string command = argv[1];

    if (command == "init") repo.init();
    else if (command == "status") repo.status();
    else if (command == "add") repo.add(argv[2]);
    else if (command == "commit") repo.commit(argv[2]);
    else if (command == "log") repo.log();
    else cout << "Unknown command" << endl;

    return 0;
}
