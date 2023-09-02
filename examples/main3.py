import time

i = 0
sum=0

t1 = time.time()

while i < 10_000_000:
    i = i+1
    sum = (i *10 / 5)*i+5
    
    if sum / 545 > 10:
        sum = 123
        
t2 = time.time()

    
print(i)
print(sum)
print("finished")

print(t2 - t1)
