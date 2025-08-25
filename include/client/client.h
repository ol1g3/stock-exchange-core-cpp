class Client {
private:
    int id;
public:
    void foo();
    bool operator ==(const Client& b){
        return this->id == b.id;
    }
};