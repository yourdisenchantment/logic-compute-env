#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
using namespace std;

const int N = 8;

class SharedInteger {
	int buf, p, c;
	mutex mx;
	condition_variable cv;
	
public:

	SharedInteger(){
		p=c=0;
	}

	void putValue(int x)
	{
		{
			unique_lock lk(mx);
			
			while(p!=c) {
				cv.wait(lk);
			}
		}
		
		buf = x;
		p++;
		cv.notify_one();
	}

	int getValue()
	{
		{
			unique_lock lk(mx);
			
			while( p ==c ) {
				cv.wait(lk);
			}
		}
		
		int y = buf;
		c++;
		cv.notify_one();
		return y;
	}
};

int main()
{
	
	
	SharedInteger s1;
	SharedInteger s2;
	
	thread producer = thread([&]{
		int A[N] = {1,4,3,6,7,8,9,4};
		
		for(int i=0;i<N;i++) {
			s1.putValue(A[i]);				
		}
	});
	
	thread consumer = thread([&]{
		int B[N];
		
		for(int i=0;i<N;i++) {
			B[i] = s1.getValue();
		}
		
		for(int i=0;i<N;i++)
			s2.putValue(B[i]*2);

	});
	
	thread consumer2 = thread([&]{
		int B[N];
		
		for(int i=0;i<N;i++) {
			B[i] = s2.getValue();
		}
		for(int i=0;i<N;i++)
			cout << B[i] << " ";
		cout << endl;
	});
	
	producer.join();
	consumer.join();
	consumer2.join();
	return 0;
}
